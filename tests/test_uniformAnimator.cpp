#include "doctest.h"
#include "UniformAnimator.hpp"

#include <variant>

// Helpers
static float getFloat(const ApiValue& v) { return std::get<float>(v); }
static std::vector<float> getVec(const ApiValue& v) {
    return std::get<std::vector<float>>(v);
}

// Advance the animator from a cold start.
// tick(0) initialises lastTime; tick(0+dt) produces the first real step.
static void coldTick(UniformAnimator& anim, float dt) {
    anim.tick(0.f);
    anim.tick(dt);
}

TEST_CASE("UniformAnimator: apply receives target value when animation completes") {
    UniformAnimator anim;
    float received = -1.f;

    anim.animateTo(0.f, 1.f, 1.f, [&](const ApiValue& v) {
        received = getFloat(v);
    });

    anim.tick(0.f);   // initialise lastTime, picks up pending animation
    anim.tick(1.f);   // dt=1.0, elapsed=1.0 — animation finishes

    CHECK(received == doctest::Approx(1.f));
}

TEST_CASE("UniformAnimator: apply is not called after animation ends") {
    UniformAnimator anim;
    int callCount = 0;

    anim.animateTo(0.f, 1.f, 1.f, [&](const ApiValue&) { ++callCount; });

    anim.tick(0.f);
    anim.tick(1.f);   // completes
    int countAtEnd = callCount;
    anim.tick(2.f);   // animation should be gone

    CHECK(callCount == countAtEnd);
}

TEST_CASE("UniformAnimator: float value is interpolated, not snapped") {
    UniformAnimator anim;
    float midpoint = -1.f;

    anim.animateTo(0.f, 1.f, 2.f, [&](const ApiValue& v) {
        midpoint = getFloat(v);
    });

    anim.tick(0.f);
    anim.tick(1.f);   // halfway through a 2s animation

    // easeInOut(0.5) == 0.5, so lerp(0,1,0.5) == 0.5
    CHECK(midpoint > 0.f);
    CHECK(midpoint < 1.f);
}

TEST_CASE("UniformAnimator: vector<float> values are interpolated element-wise") {
    UniformAnimator anim;
    std::vector<float> received;

    ApiValue from = std::vector<float>{0.f, 0.f};
    ApiValue to   = std::vector<float>{2.f, 4.f};

    anim.animateTo(from, to, 1.f, [&](const ApiValue& v) {
        received = getVec(v);
    });

    anim.tick(0.f);
    anim.tick(1.f);   // completes — should equal target

    REQUIRE(received.size() == 2);
    CHECK(received[0] == doctest::Approx(2.f));
    CHECK(received[1] == doctest::Approx(4.f));
}

TEST_CASE("UniformAnimator: zero duration snaps immediately to target") {
    UniformAnimator anim;
    float received = -1.f;

    anim.animateTo(0.f, 1.f, 0.f, [&](const ApiValue& v) {
        received = getFloat(v);
    });

    coldTick(anim, 0.001f);

    CHECK(received == doctest::Approx(1.f));
}

TEST_CASE("UniformAnimator: multiple concurrent animations fire independently") {
    UniformAnimator anim;
    float a = -1.f, b = -1.f;

    anim.animateTo(0.f, 10.f, 1.f, [&](const ApiValue& v) { a = getFloat(v); });
    anim.animateTo(0.f, 20.f, 1.f, [&](const ApiValue& v) { b = getFloat(v); });

    anim.tick(0.f);
    anim.tick(1.f);

    CHECK(a == doctest::Approx(10.f));
    CHECK(b == doctest::Approx(20.f));
}

TEST_CASE("UniformAnimator: animations queued from another thread are picked up") {
    UniformAnimator anim;
    float received = -1.f;

    anim.tick(0.f);  // initialise with no pending animations

    // Simulate an API-thread enqueue after the first tick
    anim.animateTo(0.f, 5.f, 1.f, [&](const ApiValue& v) {
        received = getFloat(v);
    });

    anim.tick(1.f);  // should pick up and complete the animation

    CHECK(received == doctest::Approx(5.f));
}
