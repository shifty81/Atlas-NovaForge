#include "GalaxyGenerator.h"
#include <cmath>
#include <algorithm>

namespace atlas::world {

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kTwoPi = 2.0f * kPi;

uint32_t GalaxyGenerator::HashSeed(uint32_t seed, uint32_t index) {
    uint32_t h = seed ^ (index * 2654435761u);
    h ^= h >> 16;
    h *= 0x45d9f3bu;
    h ^= h >> 16;
    return h;
}

float GalaxyGenerator::RandomFloat(uint32_t hash) {
    return static_cast<float>(hash & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
}

std::vector<StarSystem> GalaxyGenerator::Generate(const GalaxyParams& params) {
    std::vector<StarSystem> systems;
    systems.reserve(static_cast<size_t>(params.systemCount));

    for (int i = 0; i < params.systemCount; ++i) {
        uint32_t h0 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 0);
        uint32_t h1 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 1);
        uint32_t h2 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 2);
        uint32_t h3 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 3);
        uint32_t h4 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 4);
        uint32_t h5 = HashSeed(params.seed, static_cast<uint32_t>(i) * 7 + 5);

        // Choose arm and compute angle
        int arm = static_cast<int>(h0 % static_cast<uint32_t>(params.armCount));
        float armAngle = static_cast<float>(arm) / static_cast<float>(params.armCount) * kTwoPi;

        // Random distance from center (biased toward center)
        float r = RandomFloat(h1);
        r = r * r; // square for center bias
        float dist = r * params.galaxyRadius;

        // Spiral angle increases with distance
        float spiralAngle = armAngle + dist / params.galaxyRadius * params.armRotation;

        // Add spread/jitter
        float spread = (RandomFloat(h2) - 0.5f) * 2.0f * params.armSpread;

        // Core density boost: systems near center ignore arm structure
        bool isCore = dist < params.coreRadius * params.galaxyRadius;
        float angle = isCore
            ? RandomFloat(h3) * kTwoPi
            : spiralAngle + spread;

        double x = static_cast<double>(dist * std::cos(angle));
        double y = static_cast<double>((RandomFloat(h4) - 0.5f) * params.galaxyRadius * 0.05f); // thin disk
        double z = static_cast<double>(dist * std::sin(angle));

        StarSystem sys;
        sys.id = static_cast<uint64_t>(i);
        sys.x = x;
        sys.y = y;
        sys.z = z;
        sys.luminosity = RandomFloat(h5);
        sys.starClass = static_cast<uint8_t>(h3 % 7);
        sys.planetCount = static_cast<int>(h4 % 12);

        systems.push_back(sys);
    }

    return systems;
}

std::vector<StarSystem> GalaxyGenerator::GenerateInRegion(
    const GalaxyParams& params,
    double minX, double minY, double maxX, double maxY,
    int maxSystems)
{
    // Generate full galaxy and filter to region
    // In production, this would use spatial hashing for efficiency
    auto all = Generate(params);

    std::vector<StarSystem> result;
    for (const auto& sys : all) {
        if (sys.x >= minX && sys.x <= maxX && sys.z >= minY && sys.z <= maxY) {
            result.push_back(sys);
            if (static_cast<int>(result.size()) >= maxSystems) break;
        }
    }

    return result;
}

}
