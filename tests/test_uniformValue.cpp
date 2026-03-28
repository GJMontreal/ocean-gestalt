#include "doctest.h"
#include "UniformValue.hpp"

#include <glm/glm.hpp>

TEST_CASE("apiToUniform: float passes through") {
    ApiValue v = 3.14f;
    auto result = apiToUniform(v);
    CHECK(std::get<float>(result) == doctest::Approx(3.14f));
}

TEST_CASE("apiToUniform: int passes through") {
    ApiValue v = 42;
    auto result = apiToUniform(v);
    CHECK(std::get<int>(result) == 42);
}

TEST_CASE("apiToUniform: bool converts to int") {
    CHECK(std::get<int>(apiToUniform(ApiValue{true}))  == 1);
    CHECK(std::get<int>(apiToUniform(ApiValue{false})) == 0);
}

TEST_CASE("apiToUniform: vec2 from 2-element vector") {
    ApiValue v = std::vector<float>{1.f, 2.f};
    auto result = apiToUniform(v);
    auto vec = std::get<glm::vec2>(result);
    CHECK(vec.x == doctest::Approx(1.f));
    CHECK(vec.y == doctest::Approx(2.f));
}

TEST_CASE("apiToUniform: vec3 from 3-element vector") {
    ApiValue v = std::vector<float>{1.f, 2.f, 3.f};
    auto result = apiToUniform(v);
    auto vec = std::get<glm::vec3>(result);
    CHECK(vec.x == doctest::Approx(1.f));
    CHECK(vec.y == doctest::Approx(2.f));
    CHECK(vec.z == doctest::Approx(3.f));
}

TEST_CASE("apiToUniform: vec4 from 4-element vector") {
    ApiValue v = std::vector<float>{0.1f, 0.2f, 0.3f, 1.0f};
    auto result = apiToUniform(v);
    auto vec = std::get<glm::vec4>(result);
    CHECK(vec.r == doctest::Approx(0.1f));
    CHECK(vec.g == doctest::Approx(0.2f));
    CHECK(vec.b == doctest::Approx(0.3f));
    CHECK(vec.a == doctest::Approx(1.0f));
}

TEST_CASE("apiToUniform: invalid vector size throws") {
    ApiValue v = std::vector<float>{1.f, 2.f, 3.f, 4.f, 5.f};
    CHECK_THROWS_AS(apiToUniform(v), std::invalid_argument);
}

TEST_CASE("uniformToApi: float round-trip") {
    ApiValue original = 2.71f;
    auto u = apiToUniform(original);
    auto recovered = uniformToApi(u);
    CHECK(std::get<float>(recovered) == doctest::Approx(2.71f));
}

TEST_CASE("uniformToApi: vec3 round-trip") {
    ApiValue original = std::vector<float>{4.f, 5.f, 6.f};
    auto u = apiToUniform(original);
    auto recovered = uniformToApi(u);
    auto vec = std::get<std::vector<float>>(recovered);
    REQUIRE(vec.size() == 3);
    CHECK(vec[0] == doctest::Approx(4.f));
    CHECK(vec[1] == doctest::Approx(5.f));
    CHECK(vec[2] == doctest::Approx(6.f));
}
