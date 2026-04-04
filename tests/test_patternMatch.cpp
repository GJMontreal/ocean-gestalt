#include "doctest.h"
#include "PatternMatch.hpp"

TEST_CASE("matchesPattern: exact match with no wildcard") {
    CHECK(matchesPattern("waves[0].amplitude", "waves[0].amplitude"));
    CHECK_FALSE(matchesPattern("waves[0].amplitude", "waves[0].wavelength"));
}

TEST_CASE("matchesPattern: wildcard matches any index") {
    CHECK(matchesPattern("waves[*].amplitude", "waves[0].amplitude"));
    CHECK(matchesPattern("waves[*].amplitude", "waves[9].amplitude"));
    CHECK(matchesPattern("waves[*].amplitude", "waves[12].amplitude"));
}

TEST_CASE("matchesPattern: wildcard does not match wrong parameter") {
    CHECK_FALSE(matchesPattern("waves[*].amplitude", "waves[0].wavelength"));
    CHECK_FALSE(matchesPattern("waves[*].amplitude", "waves[0].steepness"));
    CHECK_FALSE(matchesPattern("waves[*].amplitude", "waves[0].direction"));
}

TEST_CASE("matchesPattern: wildcard does not match unrelated keys") {
    CHECK_FALSE(matchesPattern("waves[*].amplitude", "mesh_shader.causticScale"));
    CHECK_FALSE(matchesPattern("waves[*].amplitude", "buoy_mesh.ambientStrength"));
}

TEST_CASE("matchesPattern: wildcard at start") {
    CHECK(matchesPattern("*.amplitude", "waves[0].amplitude"));
    CHECK(matchesPattern("*.amplitude", "mesh_shader.amplitude"));
    CHECK_FALSE(matchesPattern("*.amplitude", "waves[0].wavelength"));
}

TEST_CASE("matchesPattern: multiple wildcards") {
    CHECK(matchesPattern("waves[*].*", "waves[0].amplitude"));
    CHECK(matchesPattern("waves[*].*", "waves[3].direction"));
    CHECK_FALSE(matchesPattern("waves[*].*", "mesh_shader.causticScale"));
}

TEST_CASE("matchesAnyPattern: returns true if any pattern matches") {
    std::vector<std::string> patterns = {"waves[*].amplitude", "waves[*].steepness"};
    CHECK(matchesAnyPattern(patterns, "waves[0].amplitude"));
    CHECK(matchesAnyPattern(patterns, "waves[4].steepness"));
    CHECK_FALSE(matchesAnyPattern(patterns, "waves[0].wavelength"));
    CHECK_FALSE(matchesAnyPattern(patterns, "mesh_shader.causticScale"));
}

TEST_CASE("matchesAnyPattern: empty pattern list matches nothing") {
    std::vector<std::string> patterns = {};
    CHECK_FALSE(matchesAnyPattern(patterns, "waves[0].amplitude"));
}
