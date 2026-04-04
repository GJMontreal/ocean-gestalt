#include "doctest.h"
#include "PathHandler.hpp"
#include "IUniformStore.hpp"

#include <cmath>
#include <string>
#include <vector>

// ---- Mock store ----
struct RecordedSet {
    std::string shader;
    std::string uniform;
    ApiValue value;
};

class MockUniformStore : public IUniformStore {
public:
    std::vector<RecordedSet> sets;
    std::unordered_map<std::string, ApiValue> stored;

    std::optional<ApiValue> setUniform(
        const std::string& shader, const std::string& uniform,
        ApiValue value, bool /*block*/) override {
        sets.push_back({shader, uniform, value});
        stored[shader + "/" + uniform] = value;
        return value;
    }

    std::optional<ApiValue> getUniform(
        const std::string& shader, const std::string& uniform) override {
        auto it = stored.find(shader + "/" + uniform);
        if (it != stored.end()) return it->second;
        return std::nullopt;
    }

    std::unique_ptr<UniformMap> dumpUniforms() override { return nullptr; }
};

// ---- Testable subclass ----
class TestableUniformPathHandler: public UniformPathHandler{
public:
  TestableUniformPathHandler():UniformPathHandler(nullptr,{"water_mesh_shader"}) {};

  using UniformPathHandler::UniformPathHandler;

  ParsedUniformPath callParseUniformPath(const std::vector<std::string>& parts){
    return parseUniformPath(parts);
  }
};

TEST_CASE("parseUniformPath handles heading substitution correctly without shadername") {
  using V = std::vector<std::string>;
  auto handler = TestableUniformPathHandler();

  auto parsed = handler.callParseUniformPath(V{"uniforms", "wave[0]", "heading"});
  CHECK(parsed.shaderName == std::nullopt);
  CHECK(parsed.internalPath == "wave[0].direction");
  CHECK(parsed.externalPath == "wave[0].heading");
  CHECK(parsed.convertHeading == true);
}

TEST_CASE("parseUniformPath handles heading substitution correctly with shadername") {
  using V = std::vector<std::string>;
  auto handler = TestableUniformPathHandler();

  auto parsed = handler.callParseUniformPath(V{"uniforms","water_mesh_shader", "wave[0]", "heading"});

  CHECK(parsed.shaderName == "water_mesh_shader");
  CHECK(parsed.internalPath == "wave[0].direction");
  CHECK(parsed.externalPath == "wave[0].heading");
  CHECK(parsed.convertHeading == true);
}

TEST_CASE("vecToHeading and headingToVec3 round-trip") {
  const float headings[] = {0.f, 45.f, 90.f, 135.f, 180.f, 270.f, 359.f};
  for (float h : headings) {
    auto vec = headingToVec3(h);
    float recovered = vecToHeading(vec[0], vec[1]);
    CHECK(recovered == doctest::Approx(h).epsilon(0.01));
  }
}

TEST_CASE("headingToVec3 produces unit vector with zero z") {
  auto vec = headingToVec3(45.f);
  REQUIRE(vec.size() == 3);
  float len = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1]);
  CHECK(len == doctest::Approx(1.f).epsilon(0.001));
  CHECK(vec[2] == doctest::Approx(0.f));
}

// ---- set() / get() tests via MockUniformStore ----

TEST_CASE("set: routes to all shaders when no shader name in path") {
  auto store = std::make_shared<MockUniformStore>();
  UniformPathHandler handler(store, {"shaderA", "shaderB"});

  handler.set({"uniforms", "uFog"}, ApiValue{1.5f}, false);

  REQUIRE(store->sets.size() == 2);
  CHECK(store->sets[0].shader == "shaderA");
  CHECK(store->sets[1].shader == "shaderB");
  CHECK(store->sets[0].uniform == "uFog");
  CHECK(std::get<float>(store->sets[0].value) == doctest::Approx(1.5f));
}

TEST_CASE("set: routes only to named shader when shader name present") {
  auto store = std::make_shared<MockUniformStore>();
  UniformPathHandler handler(store, {"shaderA", "shaderB"});

  handler.set({"uniforms", "shaderB", "uFog"}, ApiValue{2.0f}, false);

  REQUIRE(store->sets.size() == 1);
  CHECK(store->sets[0].shader == "shaderB");
  CHECK(store->sets[0].uniform == "uFog");
}

TEST_CASE("set: converts heading to direction vec3") {
  auto store = std::make_shared<MockUniformStore>();
  UniformPathHandler handler(store, {"water_mesh_shader"});

  handler.set({"uniforms", "wave[0]", "heading"}, ApiValue{0.0f}, false);

  REQUIRE(store->sets.size() == 1);
  CHECK(store->sets[0].uniform == "wave[0].direction");
  auto vec = std::get<std::vector<float>>(store->sets[0].value);
  REQUIRE(vec.size() == 3);
  CHECK(vec[0] == doctest::Approx(1.f).epsilon(0.001));
  CHECK(vec[1] == doctest::Approx(0.f).epsilon(0.001));
}

TEST_CASE("set: syncWaveFunction called for wave-related path") {
  auto store = std::make_shared<MockUniformStore>();
  UniformPathHandler handler(store, {"water_mesh_shader"});

  std::string syncedPath;
  ApiValue syncedValue{0.f};
  handler.syncWaveFunction = [&](const std::string& p, const ApiValue& v) {
    syncedPath = p;
    syncedValue = v;
  };

  handler.set({"uniforms", "waves[0]", "amplitude"}, ApiValue{1.2f}, false);

  CHECK(syncedPath == "waves[0].amplitude");
  CHECK(std::get<float>(syncedValue) == doctest::Approx(1.2f));
}

TEST_CASE("get: retrieves value from store") {
  auto store = std::make_shared<MockUniformStore>();
  store->stored["shaderA/uTime"] = ApiValue{3.0f};
  UniformPathHandler handler(store, {"shaderA"});

  auto result = handler.get({"uniforms", "uTime"});

  REQUIRE(result.has_value());
  CHECK(std::get<float>(*result) == doctest::Approx(3.0f));
}

TEST_CASE("get: converts stored direction vec3 back to heading") {
  auto store = std::make_shared<MockUniformStore>();
  // heading 90 degrees → {0, 1, 0}
  store->stored["water_mesh_shader/wave[0].direction"] =
      ApiValue{std::vector<float>{0.f, 1.f, 0.f}};
  UniformPathHandler handler(store, {"water_mesh_shader"});

  auto result = handler.get({"uniforms", "wave[0]", "heading"});

  REQUIRE(result.has_value());
  CHECK(std::get<float>(*result) == doctest::Approx(90.f).epsilon(0.01));
}