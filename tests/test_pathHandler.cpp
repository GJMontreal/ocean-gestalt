#include "doctest.h"
#include "PathHandler.hpp"

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

TEST_CASE("parseUniformPath handles heading substitution correctly with shadername") {
  using V = std::vector<std::string>;
  auto handler = TestableUniformPathHandler();

  auto parsed = handler.callParseUniformPath(V{"uniforms","mesh_shader", "wave[0]", "heading"});

  CHECK(parsed.shaderName == "mesh_shader");
  CHECK(parsed.internalPath == "wave[0].direction");
  CHECK(parsed.externalPath == "wave[0].heading");
  CHECK(parsed.convertHeading == true);
}