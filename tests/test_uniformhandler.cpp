#include <doctest.h>
#include "UniformHandler.hpp"
#include "UniformAnimator.hpp"

// Returns nullopt for everything — used where we only care about routing
class DummyApi : public ApiAdapter {
public:
  std::optional<ApiValue> setValue(const std::string&, const ApiValue&, bool) const override {
    return std::nullopt;
  }
  std::optional<ApiValue> getValue(const std::string&) const override {
    return std::nullopt;
  }
  std::optional<ApiValue> setUniform(const std::string&, const std::string&, ApiValue) override {
    return std::nullopt;
  }
  std::optional<std::any> getUniform(const std::string, const std::string) override {
    return std::nullopt;
  }
  std::unique_ptr<UniformMap> dumpUniforms() override { return nullptr; }
  void pauseSimulation(bool) override {}
  void updateSimulation(std::string, std::string) override {}
};

// Stores values and returns them — used where we need success branches to execute
class RecordingApi : public ApiAdapter {
public:
  mutable std::unordered_map<std::string, ApiValue> store;

  std::optional<ApiValue> setValue(const std::string& path, const ApiValue& value, bool) const override {
    store[path] = value;
    return value;
  }
  std::optional<ApiValue> getValue(const std::string& path) const override {
    auto it = store.find(path);
    if (it != store.end()) return it->second;
    return std::nullopt;
  }
  std::optional<ApiValue> setUniform(const std::string&, const std::string&, ApiValue) override {
    return std::nullopt;
  }
  std::optional<std::any> getUniform(const std::string, const std::string) override {
    return std::nullopt;
  }
  std::unique_ptr<UniformMap> dumpUniforms() override { return nullptr; }
  void pauseSimulation(bool) override {}
  void updateSimulation(std::string, std::string) override {}
};

class TestableUniformHandler : public UniformHandler {
public:
  using UniformHandler::UniformHandler;

  std::optional<UniformKey> callSplitPath(const std::string_view& path) {
    return splitPath(path);
  }
  std::optional<std::string> callHandleGet(const std::string& path) {
    return handleGet(path);
  }
  std::optional<std::string> callHandlePost(const std::string& path,
                                             nlohmann::json::value_type value,
                                             std::optional<float> duration) {
    return handlePost(path, value, duration);
  }
};

// ---- splitPath ----

TEST_CASE("UniformHandler::splitPath handles valid and invalid paths") {
  DummyApi api;
  TestableUniformHandler handler(api, nullptr);

  SUBCASE("valid path") {
    auto parsed = handler.callSplitPath("shaderOne/uTime");
    REQUIRE(parsed.has_value());
    CHECK(parsed->shaderName == "shaderOne");
    CHECK(parsed->uniformName == "uTime");
  }

  SUBCASE("missing slash") {
    auto parsed = handler.callSplitPath("shaderOnly");
    CHECK_FALSE(parsed.has_value());
  }

  SUBCASE("empty path") {
    auto parsed = handler.callSplitPath("");
    CHECK_FALSE(parsed.has_value());
  }

  SUBCASE("multiple slashes") {
    auto parsed = handler.callSplitPath("shader/frag/time");
    REQUIRE(parsed.has_value());
    CHECK(parsed->shaderName == "shader");
    CHECK(parsed->uniformName == "frag/time");
  }
}

// ---- handleGet ----

TEST_CASE("handleGet: returns nullopt when path not found") {
  DummyApi api;
  TestableUniformHandler handler(api, nullptr);
  CHECK_FALSE(handler.callHandleGet("uniforms.missing").has_value());
}

TEST_CASE("handleGet: returns serialised value when api has it") {
  RecordingApi api;
  api.store["uniforms.uFog"] = ApiValue{1.5f};
  TestableUniformHandler handler(api, nullptr);
  auto result = handler.callHandleGet("uniforms.uFog");
  REQUIRE(result.has_value());
  CHECK(*result == "1.5");
}

TEST_CASE("handleGet: serialises vec3 value") {
  RecordingApi api;
  api.store["uniforms.uColor"] = ApiValue{std::vector<float>{0.1f, 0.5f, 0.9f}};
  TestableUniformHandler handler(api, nullptr);
  auto result = handler.callHandleGet("uniforms.uColor");
  REQUIRE(result.has_value());
  auto parsed = nlohmann::json::parse(*result);
  REQUIRE(parsed.is_array());
  CHECK(parsed[0].get<float>() == doctest::Approx(0.1f));
}

// ---- handlePost: type dispatch ----

TEST_CASE("handlePost: float value stored and returned") {
  RecordingApi api;
  TestableUniformHandler handler(api, nullptr);
  auto result = handler.callHandlePost("uniforms.uFog", nlohmann::json(1.5f), std::nullopt);
  REQUIRE(result.has_value());
  REQUIRE(api.store.count("uniforms.uFog"));
  CHECK(std::get<float>(api.store.at("uniforms.uFog")) == doctest::Approx(1.5f));
}

TEST_CASE("handlePost: array value stored") {
  RecordingApi api;
  TestableUniformHandler handler(api, nullptr);
  nlohmann::json val = nlohmann::json::array({0.1f, 0.5f, 0.9f});
  auto result = handler.callHandlePost("uniforms.uColor", val, std::nullopt);
  REQUIRE(result.has_value());
  REQUIRE(api.store.count("uniforms.uColor"));
  auto& stored = std::get<std::vector<float>>(api.store.at("uniforms.uColor"));
  CHECK(stored[0] == doctest::Approx(0.1f));
  CHECK(stored[2] == doctest::Approx(0.9f));
}

TEST_CASE("handlePost: bool value stored as bool") {
  RecordingApi api;
  TestableUniformHandler handler(api, nullptr);
  auto result = handler.callHandlePost("uniforms.uEnabled", nlohmann::json(true), std::nullopt);
  REQUIRE(result.has_value());
  REQUIRE(api.store.count("uniforms.uEnabled"));
  CHECK(std::get<bool>(api.store.at("uniforms.uEnabled")) == true);
}

TEST_CASE("handlePost: unrecognised type returns nullopt") {
  RecordingApi api;
  TestableUniformHandler handler(api, nullptr);
  // plain integer — not in the handler list
  auto result = handler.callHandlePost("uniforms.uFog", nlohmann::json(42), std::nullopt);
  CHECK_FALSE(result.has_value());
}

// ---- handlePost: animator branch ----

TEST_CASE("handlePost: with duration queues animation instead of direct set") {
  RecordingApi api;
  auto animator = std::make_shared<UniformAnimator>();
  TestableUniformHandler handler(api, animator);

  auto result = handler.callHandlePost("uniforms.uFog", nlohmann::json(2.0f), 1.0f);

  REQUIRE(result.has_value());
  // setValue should NOT have been called yet — value arrives via tick
  CHECK_FALSE(api.store.count("uniforms.uFog"));

  // After ticking past duration, the value should have been applied
  animator->tick(0.0f);
  animator->tick(2.0f);
  REQUIRE(api.store.count("uniforms.uFog"));
  CHECK(std::get<float>(api.store.at("uniforms.uFog")) == doctest::Approx(2.0f));
}

TEST_CASE("handlePost: duration with no animator falls through to direct set") {
  RecordingApi api;
  TestableUniformHandler handler(api, nullptr);  // no animator
  auto result = handler.callHandlePost("uniforms.uFog", nlohmann::json(3.0f), 1.0f);
  REQUIRE(result.has_value());
  REQUIRE(api.store.count("uniforms.uFog"));
  CHECK(std::get<float>(api.store.at("uniforms.uFog")) == doctest::Approx(3.0f));
}
