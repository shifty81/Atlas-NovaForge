#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <cstdint>

namespace atlas::interaction {

using SlotValue = std::variant<
    int64_t,
    double,
    bool,
    std::string
>;

struct Intent {
    std::string name;
    float confidence = 1.0f;
    std::unordered_map<std::string, SlotValue> slots;
};

} // namespace atlas::interaction
