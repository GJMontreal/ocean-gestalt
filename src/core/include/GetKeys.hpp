#pragma once

#include <vector>

template <typename Map>
std::vector<typename Map::key_type> getKeys(const Map& m) {
    std::vector<typename Map::key_type> keys;
    keys.reserve(m.size());
    for (const auto& pair : m) {
        keys.push_back(pair.first);
    }
    return keys;
}
