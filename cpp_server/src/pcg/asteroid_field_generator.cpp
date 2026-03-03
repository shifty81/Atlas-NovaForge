#include "pcg/asteroid_field_generator.h"
#include "pcg/hash_utils.h"

#include <algorithm>
#include <cmath>

namespace atlas {
namespace pcg {

// ── Constants ──────────────────────────────────────────────────────
static constexpr int   MIN_ASTEROIDS   = 10;
static constexpr int   MAX_ASTEROIDS   = 80;
static constexpr float DEFAULT_RADIUS  = 200.0f;  // field bounding radius (m)
static constexpr float TWO_PI          = 6.2831853f;

// ── Public API ─────────────────────────────────────────────────────

AsteroidField AsteroidFieldGenerator::generate(const PCGContext& ctx,
                                               float secLevel) {
    DeterministicRNG rng(ctx.seed);
    int count = rng.range(MIN_ASTEROIDS, MAX_ASTEROIDS);
    return generate(ctx, count, secLevel);
}

AsteroidField AsteroidFieldGenerator::generate(const PCGContext& ctx,
                                               int asteroidCount,
                                               float secLevel) {
    DeterministicRNG rng(ctx.seed);
    // Consume the same first roll to keep RNG state consistent.
    (void)rng.range(MIN_ASTEROIDS, MAX_ASTEROIDS);

    int count = std::max(MIN_ASTEROIDS, std::min(asteroidCount, MAX_ASTEROIDS));

    float fieldRadius = DEFAULT_RADIUS + rng.rangeFloat(0.0f, 100.0f);

    AsteroidField field{};
    field.fieldId        = ctx.seed;
    field.fieldRadius    = fieldRadius;
    field.totalAsteroids = count;
    field.totalYield     = 0.0f;

    field.asteroids.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i) {
        AsteroidNode node{};
        node.asteroidId  = i;
        node.type        = selectType(rng, secLevel);
        node.radius      = rollRadius(rng, node.type);
        node.mineralYield = rollYield(rng, node.type);

        // Scatter in a disc (Y is "up", XZ is the belt plane).
        float angle  = rng.rangeFloat(0.0f, TWO_PI);
        float dist   = rng.rangeFloat(0.0f, fieldRadius);
        node.posX = dist * std::cos(angle);
        node.posY = rng.rangeFloat(-20.0f, 20.0f);  // slight vertical spread
        node.posZ = dist * std::sin(angle);

        // Default scale factor: normalise to a 10 m reference mesh.
        constexpr float REFERENCE_ROCK_RADIUS = 10.0f;
        node.scaleFactor = node.radius / REFERENCE_ROCK_RADIUS;

        field.totalYield += node.mineralYield;
        field.asteroids.push_back(node);
    }

    return field;
}

AsteroidField AsteroidFieldGenerator::generate(const PCGContext& ctx,
                                               int asteroidCount,
                                               float secLevel,
                                               const std::string& rockMeshArchive) {
    AsteroidField field = generate(ctx, asteroidCount, secLevel);

    // Tag every asteroid with the reference rock mesh archive.
    for (auto& a : field.asteroids) {
        a.meshArchive = rockMeshArchive;
    }

    return field;
}

float AsteroidFieldGenerator::calculateTotalYield(const AsteroidField& field) {
    float total = 0.0f;
    for (const auto& a : field.asteroids) {
        total += a.mineralYield;
    }
    return total;
}

// ── Internals ──────────────────────────────────────────────────────

AsteroidType AsteroidFieldGenerator::selectType(DeterministicRNG& rng,
                                                float secLevel) {
    // High-sec (secLevel ≥ 0.5): mostly common ores.
    // Low-sec  (0.2 < sec < 0.5): uncommon + rare ores.
    // Null-sec (sec ≤ 0.2): rare + very-rare ores.
    float roll = rng.nextFloat();

    if (secLevel >= 0.5f) {
        if (roll < 0.40f) return AsteroidType::Ferrite;
        if (roll < 0.75f) return AsteroidType::Galvite;
        if (roll < 0.90f) return AsteroidType::Cryolite;
        return AsteroidType::Silvane;
    }

    if (secLevel > 0.2f) {
        if (roll < 0.20f) return AsteroidType::Ferrite;
        if (roll < 0.40f) return AsteroidType::Galvite;
        if (roll < 0.55f) return AsteroidType::Cryolite;
        if (roll < 0.70f) return AsteroidType::Silvane;
        if (roll < 0.85f) return AsteroidType::Duskite;
        return AsteroidType::Heliore;
    }

    // Null-sec
    if (roll < 0.10f) return AsteroidType::Ferrite;
    if (roll < 0.20f) return AsteroidType::Galvite;
    if (roll < 0.30f) return AsteroidType::Cryolite;
    if (roll < 0.40f) return AsteroidType::Silvane;
    if (roll < 0.55f) return AsteroidType::Duskite;
    if (roll < 0.70f) return AsteroidType::Heliore;
    if (roll < 0.88f) return AsteroidType::Jaspet;
    return AsteroidType::Mercoxit;
}

float AsteroidFieldGenerator::rollRadius(DeterministicRNG& rng,
                                         AsteroidType type) {
    switch (type) {
        case AsteroidType::Ferrite:    return rng.rangeFloat(5.0f, 30.0f);
        case AsteroidType::Galvite:    return rng.rangeFloat(5.0f, 25.0f);
        case AsteroidType::Cryolite:   return rng.rangeFloat(8.0f, 35.0f);
        case AsteroidType::Silvane: return rng.rangeFloat(8.0f, 30.0f);
        case AsteroidType::Duskite:       return rng.rangeFloat(10.0f, 40.0f);
        case AsteroidType::Heliore:     return rng.rangeFloat(10.0f, 45.0f);
        case AsteroidType::Jaspet:      return rng.rangeFloat(12.0f, 50.0f);
        case AsteroidType::Mercoxit:    return rng.rangeFloat(15.0f, 60.0f);
    }
    return 10.0f;
}

float AsteroidFieldGenerator::rollYield(DeterministicRNG& rng,
                                        AsteroidType type) {
    switch (type) {
        case AsteroidType::Ferrite:    return rng.rangeFloat(500.0f, 2000.0f);
        case AsteroidType::Galvite:    return rng.rangeFloat(400.0f, 1800.0f);
        case AsteroidType::Cryolite:   return rng.rangeFloat(300.0f, 1500.0f);
        case AsteroidType::Silvane: return rng.rangeFloat(300.0f, 1200.0f);
        case AsteroidType::Duskite:       return rng.rangeFloat(200.0f, 1000.0f);
        case AsteroidType::Heliore:     return rng.rangeFloat(150.0f, 800.0f);
        case AsteroidType::Jaspet:      return rng.rangeFloat(100.0f, 600.0f);
        case AsteroidType::Mercoxit:    return rng.rangeFloat(50.0f, 300.0f);
    }
    return 100.0f;
}

} // namespace pcg
} // namespace atlas
