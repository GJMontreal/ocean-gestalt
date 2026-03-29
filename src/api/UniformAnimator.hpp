#pragma once

#include "ApiValue.hpp"
#include <functional>
#include <mutex>
#include <vector>

class UniformState;

struct PendingAnimation {
    std::string key;  // empty = no replacement check
    ApiValue from;
    ApiValue to;
    float duration;
    std::function<void(const ApiValue&)> apply;
};

struct ActiveAnimation {
    std::string key;
    ApiValue from;
    ApiValue to;
    float elapsed = 0.f;
    float duration;
    std::function<void(const ApiValue&)> apply;
};

class UniformAnimator {
public:
    // Thread-safe — may be called from any thread.
    // If key is non-empty and an animation with the same key is already
    // running, it is replaced (starting from its current interpolated value).
    void animateTo(ApiValue from, ApiValue to, float duration,
                   std::function<void(const ApiValue&)> apply,
                   std::string key = {});

    // Thread-safe — cancels any pending or active animation with the given key.
    void cancel(const std::string& key);

    // Render thread only — advances all active animations and fires apply callbacks
    void tick(float time);

private:
    static ApiValue lerpValue(const ApiValue& a, const ApiValue& b, float t);
    static float easeInOut(float t);

    float lastTime = -1.f;
    std::mutex mutex;
    std::vector<PendingAnimation> pending;   // written by any thread
    std::vector<ActiveAnimation> active;     // render thread only
};
