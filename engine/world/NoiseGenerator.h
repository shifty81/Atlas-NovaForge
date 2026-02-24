#pragma once
#include <cstdint>

namespace atlas::world {

class NoiseGenerator {
public:
    // Classic Perlin noise (2D), returns value in [-1, 1]
    static float Perlin2D(float x, float y, uint32_t seed = 0);

    // Fractal Brownian Motion (2D) using Perlin noise
    // octaves: number of noise layers
    // lacunarity: frequency multiplier per octave (typically 2.0)
    // persistence: amplitude multiplier per octave (typically 0.5)
    static float FBM2D(float x, float y, int octaves, float lacunarity, float persistence, uint32_t seed = 0);

private:
    static float Fade(float t);
    static float Lerp(float a, float b, float t);
    static float Grad(int hash, float x, float y);
    static int Hash(int x, int y, uint32_t seed);
};

}
