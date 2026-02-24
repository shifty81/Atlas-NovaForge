#include "NoiseGenerator.h"
#include <cmath>

namespace atlas::world {

float NoiseGenerator::Fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float NoiseGenerator::Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float NoiseGenerator::Grad(int hash, float x, float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

int NoiseGenerator::Hash(int x, int y, uint32_t seed) {
    // Simple hash combining coordinates and seed
    uint32_t h = seed;
    h ^= static_cast<uint32_t>(x) * 374761393u;
    h ^= static_cast<uint32_t>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return static_cast<int>(h & 0xFF);
}

float NoiseGenerator::Perlin2D(float x, float y, uint32_t seed) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));

    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = Fade(xf);
    float v = Fade(yf);

    int h00 = Hash(xi,     yi,     seed);
    int h10 = Hash(xi + 1, yi,     seed);
    int h01 = Hash(xi,     yi + 1, seed);
    int h11 = Hash(xi + 1, yi + 1, seed);

    float g00 = Grad(h00, xf,       yf);
    float g10 = Grad(h10, xf - 1.0f, yf);
    float g01 = Grad(h01, xf,       yf - 1.0f);
    float g11 = Grad(h11, xf - 1.0f, yf - 1.0f);

    float x0 = Lerp(g00, g10, u);
    float x1 = Lerp(g01, g11, u);

    return Lerp(x0, x1, v);
}

float NoiseGenerator::FBM2D(float x, float y, int octaves, float lacunarity, float persistence, uint32_t seed) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxAmplitude = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += Perlin2D(x * frequency, y * frequency, seed + static_cast<uint32_t>(i)) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return maxAmplitude > 0.0f ? total / maxAmplitude : 0.0f;
}

}
