#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace atlas::world {

struct StarSystem {
    uint64_t id;
    double x, y, z;          // galactic coordinates
    float luminosity;         // relative brightness [0, 1]
    uint8_t starClass;        // 0=O, 1=B, 2=A, 3=F, 4=G, 5=K, 6=M
    int planetCount;
};

struct GalaxyParams {
    uint32_t seed = 42;
    int armCount = 4;               // spiral arms
    float armSpread = 0.4f;         // radial spread of each arm
    float armRotation = 2.5f;       // total rotation in radians over radius
    float coreRadius = 0.15f;       // fraction of galaxy radius for dense core
    float galaxyRadius = 50000.0f;  // light-years
    int systemCount = 10000;        // total star systems to generate
};

class GalaxyGenerator {
public:
    // Generate a full galaxy from parameters
    static std::vector<StarSystem> Generate(const GalaxyParams& params);

    // Generate systems in a spatial region (for streaming)
    static std::vector<StarSystem> GenerateInRegion(
        const GalaxyParams& params,
        double minX, double minY, double maxX, double maxY,
        int maxSystems
    );

private:
    // Deterministic pseudo-random from seed
    static uint32_t HashSeed(uint32_t seed, uint32_t index);
    static float RandomFloat(uint32_t hash);
};

}
