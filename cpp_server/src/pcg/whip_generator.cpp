#include "pcg/whip_generator.h"

#include <algorithm>

namespace atlas {
namespace pcg {

// ── Static helpers ──────────────────────────────────────────────────

float WhipGenerator::computeBaseReach(WhipStyle style) {
    switch (style) {
        case WhipStyle::Energy:      return 2500.0f;  // Plasma tendril
        case WhipStyle::Kinetic:     return 2000.0f;  // Physical cable
        case WhipStyle::Thermal:     return 3000.0f;  // Superheated filament
        case WhipStyle::Gravimetric: return 3500.0f;  // Gravity tether
    }
    return 2000.0f;
}

int WhipGenerator::computeTendrilCount(uint64_t seed, WhipStyle style) {
    DeterministicRNG rng(seed);
    switch (style) {
        case WhipStyle::Energy:      return rng.range(1, 3);
        case WhipStyle::Kinetic:     return rng.range(1, 2);
        case WhipStyle::Thermal:     return rng.range(2, 4);
        case WhipStyle::Gravimetric: return rng.range(1, 2);
    }
    return 1;
}

const char* WhipGenerator::styleName(WhipStyle style) {
    switch (style) {
        case WhipStyle::Energy:      return "Energy";
        case WhipStyle::Kinetic:     return "Kinetic";
        case WhipStyle::Thermal:     return "Thermal";
        case WhipStyle::Gravimetric: return "Gravimetric";
    }
    return "Unknown";
}

// ── Public API ──────────────────────────────────────────────────────

GeneratedWhip WhipGenerator::generate(uint64_t seed, WhipStyle style,
                                       const std::string& faction) const {
    DeterministicRNG rng(seed);

    GeneratedWhip whip{};
    whip.whip_id = seed;
    whip.valid   = true;

    whip.profile.style          = style;
    whip.profile.faction_style  = faction;
    whip.profile.reach          = computeBaseReach(style) * rng.rangeFloat(0.8f, 1.2f);
    whip.profile.tendril_count  = computeTendrilCount(seed, style);
    whip.profile.arc_degrees    = rng.rangeFloat(180.0f, 360.0f);

    // Base stats by style
    switch (style) {
        case WhipStyle::Energy:
            whip.profile.base_damage    = rng.rangeFloat(80.0f, 150.0f);
            whip.profile.swing_time     = rng.rangeFloat(2.0f, 4.0f);
            whip.profile.tracking_speed = rng.rangeFloat(0.10f, 0.20f);
            break;
        case WhipStyle::Kinetic:
            whip.profile.base_damage    = rng.rangeFloat(120.0f, 200.0f);
            whip.profile.swing_time     = rng.rangeFloat(3.0f, 5.0f);
            whip.profile.tracking_speed = rng.rangeFloat(0.06f, 0.12f);
            break;
        case WhipStyle::Thermal:
            whip.profile.base_damage    = rng.rangeFloat(100.0f, 180.0f);
            whip.profile.swing_time     = rng.rangeFloat(2.5f, 4.5f);
            whip.profile.tracking_speed = rng.rangeFloat(0.08f, 0.16f);
            break;
        case WhipStyle::Gravimetric:
            whip.profile.base_damage    = rng.rangeFloat(40.0f, 80.0f);
            whip.profile.swing_time     = rng.rangeFloat(4.0f, 6.0f);
            whip.profile.tracking_speed = rng.rangeFloat(0.05f, 0.10f);
            break;
    }

    // Fitting costs
    whip.power_draw           = rng.rangeFloat(15.0f, 40.0f);
    whip.cpu_usage            = rng.rangeFloat(10.0f, 25.0f);
    whip.capacitor_per_cycle  = rng.rangeFloat(20.0f, 60.0f);
    whip.tendril_length       = whip.profile.reach * rng.rangeFloat(0.6f, 1.0f);
    whip.base_width           = rng.rangeFloat(0.5f, 2.0f);
    whip.mass                 = rng.rangeFloat(500.0f, 2000.0f);

    // Faction modifiers
    if (faction == "Solari") {
        whip.profile.base_damage *= 1.15f;   // Solari favour raw power
        whip.profile.swing_time  *= 1.05f;   // Slightly slower
    } else if (faction == "Veyren") {
        whip.profile.tracking_speed *= 1.10f; // Veyren are agile
        whip.profile.swing_time     *= 0.90f; // Faster swings
    } else if (faction == "Aurelian") {
        whip.profile.arc_degrees = std::min(whip.profile.arc_degrees * 1.15f, 360.0f);
        whip.profile.tracking_speed *= 1.20f; // Best tracking
    } else if (faction == "Keldari") {
        whip.profile.reach *= 1.25f;          // Keldari reach further
        whip.power_draw    *= 0.85f;          // More efficient
    }

    return whip;
}

GeneratedWhip WhipGenerator::generateRandom(uint64_t seed,
                                             const std::string& faction) const {
    DeterministicRNG rng(seed);
    int styleIdx = rng.range(0, 3);
    WhipStyle style = static_cast<WhipStyle>(styleIdx);
    // Derive a child seed so the style selection doesn't consume RNG state
    // that the generate() call needs.
    uint64_t childSeed = rng.nextU32();
    childSeed = (childSeed << 32) | rng.nextU32();
    return generate(childSeed, style, faction);
}

} // namespace pcg
} // namespace atlas
