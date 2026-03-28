#include "doctest.h"
#include "Wave.hpp"

#include <memory>
#include <vector>

static std::vector<std::shared_ptr<Wave>> makeWaves(int count) {
    std::vector<std::shared_ptr<Wave>> waves;
    for (int i = 0; i < count; ++i)
        waves.push_back(std::make_shared<Wave>());
    return waves;
}

TEST_CASE("setWaveParameters: sets amplitude") {
    auto waves = makeWaves(3);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[1].amplitude", ApiValue{2.5f});
    CHECK(waves[1]->amplitude == doctest::Approx(2.5f));
    CHECK(waves[0]->amplitude == doctest::Approx(0.f));  // others untouched
}

TEST_CASE("setWaveParameters: sets wavelength") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[0].wavelength", ApiValue{50.f});
    CHECK(waves[0]->wavelength == doctest::Approx(50.f));
}

TEST_CASE("setWaveParameters: sets steepness") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[0].steepness", ApiValue{0.75f});
    CHECK(waves[0]->steepness == doctest::Approx(0.75f));
}

TEST_CASE("setWaveParameters: sets phase") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[0].phase", ApiValue{1.57f});
    CHECK(waves[0]->phase == doctest::Approx(1.57f));
}

TEST_CASE("setWaveParameters: sets direction from vec3") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[0].direction",
                         ApiValue{std::vector<float>{1.f, 0.f, 0.f}});
    CHECK(waves[0]->direction.x == doctest::Approx(1.f));
    CHECK(waves[0]->direction.y == doctest::Approx(0.f));
}

TEST_CASE("setWaveParameters: out-of-bounds index is silently ignored") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "waves[5].amplitude", ApiValue{9.f});
    CHECK(waves[0]->amplitude == doctest::Approx(0.f));
    CHECK(waves[1]->amplitude == doctest::Approx(0.f));
}

TEST_CASE("setWaveParameters: malformed path is silently ignored") {
    auto waves = makeWaves(2);
    WaveInterface wi;
    wi.setWaveParameters(waves, "amplitude", ApiValue{9.f});  // missing index part
    CHECK(waves[0]->amplitude == doctest::Approx(0.f));
}
