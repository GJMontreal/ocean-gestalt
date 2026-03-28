#include "UniformAnimator.hpp"

#include <algorithm>
#include <cmath>

void UniformAnimator::animateTo(ApiValue from, ApiValue to, float duration,
                                std::function<void(const ApiValue&)> apply,
                                std::string key) {
    std::lock_guard<std::mutex> lock(mutex);
    pending.push_back({std::move(key), std::move(from), std::move(to), duration, std::move(apply)});
}

void UniformAnimator::tick(float time) {
    float dt = 0.f;
    if (lastTime >= 0.f && time > lastTime) {
        dt = time - lastTime;
    }
    lastTime = time;

    // Pick up any animations queued by other threads
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& p : pending) {
            bool replaced = false;
            if (!p.key.empty()) {
                for (auto& a : active) {
                    if (a.key == p.key) {
                        float t = easeInOut(std::min(a.elapsed / a.duration, 1.f));
                        a.from     = lerpValue(a.from, a.to, t);
                        a.to       = p.to;
                        a.elapsed  = 0.f;
                        a.duration = p.duration;
                        a.apply    = std::move(p.apply);
                        replaced   = true;
                        break;
                    }
                }
            }
            if (!replaced) {
                active.push_back({p.key, p.from, p.to, 0.f, p.duration, std::move(p.apply)});
            }
        }
        pending.clear();
    }

    for (auto& anim : active) {
        anim.elapsed += dt;
        float t = (anim.duration > 0.f)
                  ? std::min(anim.elapsed / anim.duration, 1.f)
                  : 1.f;
        anim.apply(lerpValue(anim.from, anim.to, easeInOut(t)));
    }

    active.erase(
        std::remove_if(active.begin(), active.end(),
                       [](const ActiveAnimation& a) { return a.elapsed >= a.duration; }),
        active.end());
}

float UniformAnimator::easeInOut(float t) {
    return t * t * (3.f - 2.f * t);  // smoothstep
}

ApiValue UniformAnimator::lerpValue(const ApiValue& a, const ApiValue& b, float t) {
    return std::visit([&](auto&& from) -> ApiValue {
        using T = std::decay_t<decltype(from)>;
        if constexpr (std::is_same_v<T, float>) {
            float to = std::get<float>(b);
            return from + (to - from) * t;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            const auto& to = std::get<std::vector<float>>(b);
            std::vector<float> result(from.size());
            for (size_t i = 0; i < from.size() && i < to.size(); ++i) {
                result[i] = from[i] + (to[i] - from[i]) * t;
            }
            return result;
        } else {
            return t >= 1.f ? b : a;
        }
    }, a);
}
