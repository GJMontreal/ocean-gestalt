#include <doctest.h>
#include "UniformHandler.hpp"
// #include "DummyApiAdapter.hpp"

// Minimal no-op adapter since UniformHandler needs a reference
class DummyApi : public ApiAdapter {
public:
  std::optional<ApiValue> setValue(const std::string&, const ApiValue&, bool block) const override {
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
  
  std::unique_ptr<UniformMap> dumpUniforms() override { return nullptr;};
  
  void pauseSimulation(bool) override {}
  void updateSimulation(std::string, std::string) override {}
};

class TestableUniformHandler : public UniformHandler {
public:
  using UniformHandler::UniformHandler;

  std::optional<UniformKey> callSplitPath(const std::string_view& path) {
    return splitPath(path);
  }
};

TEST_CASE("UniformHandler::splitPath handles valid and invalid paths") {
  DummyApi api;
  TestableUniformHandler handler(api);

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
