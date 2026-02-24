#pragma once
#include <vector>
#include <cstdint>

namespace atlas::asset {

struct DamageEvent {
    float magnitude;
    float radius;
    uint32_t hitRegion;
};

struct DamageState {
    std::vector<DamageEvent> events;

    void Apply(const DamageEvent& e) { events.push_back(e); }
    void Clear() { events.clear(); }
    size_t EventCount() const { return events.size(); }
};

} // namespace atlas::asset
