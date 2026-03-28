#pragma once
#include <chrono>
#include <string>

class FPSCounter {
 public:
  FPSCounter() : frameCount(0), fps(0) {
    lastTime = std::chrono::high_resolution_clock::now();
  }

  void update() {
    using clock = std::chrono::high_resolution_clock;

    ++frameCount;
    auto now = clock::now();
    auto delta =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime)
            .count();

    if (delta >= 1000) {
      fps = frameCount * 1000 / delta;
      frameCount = 0;
      lastTime = now;
    }
  }

  int getFPS() const { return fps; }

  std::string getFPSString() const { return "FPS: " + std::to_string(fps); }

 private:
  int frameCount;
  int fps;
  std::chrono::high_resolution_clock::time_point lastTime;
};
