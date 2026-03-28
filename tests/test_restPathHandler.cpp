#include "doctest.h"
#include "UniformHandler.hpp"  // pulls in src/rest/PathHandler.hpp via relative include

TEST_CASE("normalisePath: strips /api/ prefix and replaces slashes with dots") {
  CHECK(PathHandler::normalisePath("/api/uniforms/mesh_shader/uFog") ==
        "uniforms.mesh_shader.uFog");
}

TEST_CASE("normalisePath: single segment") {
  CHECK(PathHandler::normalisePath("/api/uniforms") == "uniforms");
}

TEST_CASE("parsePost: valid body returns path and value") {
  auto req = PathHandler::parsePost(R"({"path":"uniforms.uFog","value":1.5})");
  REQUIRE(req.has_value());
  CHECK(req->path == "uniforms.uFog");
  CHECK(req->value == 1.5);
  CHECK_FALSE(req->duration.has_value());
}

TEST_CASE("parsePost: extracts optional duration") {
  auto req = PathHandler::parsePost(
      R"({"path":"uniforms.uFog","value":1.5,"duration":2.0})");
  REQUIRE(req.has_value());
  REQUIRE(req->duration.has_value());
  CHECK(*req->duration == doctest::Approx(2.0f));
}

TEST_CASE("parsePost: non-number duration is ignored") {
  auto req = PathHandler::parsePost(
      R"({"path":"p","value":1.0,"duration":"fast"})");
  REQUIRE(req.has_value());
  CHECK_FALSE(req->duration.has_value());
}

TEST_CASE("parsePost: missing value returns nullopt") {
  auto req = PathHandler::parsePost(R"({"path":"uniforms.uFog"})");
  CHECK_FALSE(req.has_value());
}

TEST_CASE("parsePost: missing path returns nullopt") {
  auto req = PathHandler::parsePost(R"({"value":1.5})");
  CHECK_FALSE(req.has_value());
}

TEST_CASE("parsePost: invalid JSON returns nullopt") {
  auto req = PathHandler::parsePost("not valid json");
  CHECK_FALSE(req.has_value());
}

TEST_CASE("parsePost: empty body returns nullopt") {
  auto req = PathHandler::parsePost("");
  CHECK_FALSE(req.has_value());
}

TEST_CASE("parsePost: vec3 value is preserved") {
  auto req = PathHandler::parsePost(
      R"({"path":"uniforms.uColor","value":[0.1,0.5,0.9]})");
  REQUIRE(req.has_value());
  REQUIRE(req->value.is_array());
  CHECK(req->value.size() == 3);
  CHECK(req->value[0].get<float>() == doctest::Approx(0.1f));
}
