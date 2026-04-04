#pragma once

#include <string>
#include <vector>

// Returns true if key matches pattern, where '*' in pattern matches any substring.
inline bool matchesPattern(const std::string& pattern, const std::string& key) {
    size_t ki = 0;
    size_t pi = 0;
    size_t starPos = std::string::npos;
    size_t matchPos = 0;

    while (ki < key.size()) {
        if (pi < pattern.size() && pattern[pi] == '*') {
            starPos = pi++;
            matchPos = ki;
        } else if (pi < pattern.size() && pattern[pi] == key[ki]) {
            ++pi;
            ++ki;
        } else if (starPos != std::string::npos) {
            pi = starPos + 1;
            ki = ++matchPos;
        } else {
            return false;
        }
    }

    while (pi < pattern.size() && pattern[pi] == '*') ++pi;
    return pi == pattern.size();
}

inline bool matchesAnyPattern(const std::vector<std::string>& patterns, const std::string& key) {
    for (const auto& pattern : patterns) {
        if (matchesPattern(pattern, key)) return true;
    }
    return false;
}
