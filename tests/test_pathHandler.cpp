#include "doctest.h"
#include "PathHandler.hpp"

#include <cmath>
#include <string>
#include <vector>

class TestableUniformPathHandler: public UniformPathHandler{
public:
  TestableUniformPathHandler():UniformPathHandler(nullptr,{"mesh_shader"}) {};

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

  auto parsed = handler.callParseUniformPath(V{"uniforms","mesh_shader", "wave[0]", "heading"});

  CHECK(parsed.shaderName == "mesh_shader");
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