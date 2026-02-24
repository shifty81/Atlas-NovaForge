#include "TileNodes.h"
#include <algorithm>
#include <cmath>

namespace atlas::tile {

// Integer hash noise using Squirrel3-style mixing with large primes for avalanche.
static float HashNoise(int x, int y, uint32_t seed) {
    uint32_t h = seed;
    h ^= static_cast<uint32_t>(x) * 374761393u;
    h ^= static_cast<uint32_t>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h = h ^ (h >> 16);
    return static_cast<float>(h & 0xFFFFu) / 65535.0f;
}

static float SmoothNoise(float fx, float fy, uint32_t seed) {
    int ix = static_cast<int>(std::floor(fx));
    int iy = static_cast<int>(std::floor(fy));
    float fracX = fx - static_cast<float>(ix);
    float fracY = fy - static_cast<float>(iy);

    float v00 = HashNoise(ix, iy, seed);
    float v10 = HashNoise(ix + 1, iy, seed);
    float v01 = HashNoise(ix, iy + 1, seed);
    float v11 = HashNoise(ix + 1, iy + 1, seed);

    float i0 = v00 + (v10 - v00) * fracX;
    float i1 = v01 + (v11 - v01) * fracX;
    return i0 + (i1 - i0) * fracY;
}

// --- BaseTileNode ---

std::vector<TilePort> BaseTileNode::Outputs() const {
    return {{"TileID", TilePinType::TileID}};
}

void BaseTileNode::Evaluate(const TileGenContext& /*ctx*/, const std::vector<TileValue>& /*inputs*/, std::vector<TileValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = TilePinType::TileID;
    outputs[0].data = {tileID};
}

// --- NoiseFieldNode ---

std::vector<TilePort> NoiseFieldNode::Inputs() const {
    return {
        {"Seed", TilePinType::Seed},
        {"Frequency", TilePinType::Float}
    };
}

std::vector<TilePort> NoiseFieldNode::Outputs() const {
    return {{"Field", TilePinType::Float}};
}

void NoiseFieldNode::Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const {
    uint32_t seed = ctx.seed;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        seed = static_cast<uint32_t>(inputs[0].data[0]);
    }

    float frequency = 0.1f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        frequency = inputs[1].data[0];
    }

    int w = ctx.mapWidth;
    int h = ctx.mapHeight;
    int fieldSize = w * h;

    outputs.resize(1);
    outputs[0].type = TilePinType::Float;
    outputs[0].data.resize(static_cast<size_t>(fieldSize));

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float fx = static_cast<float>(x) * frequency;
            float fy = static_cast<float>(y) * frequency;
            outputs[0].data[static_cast<size_t>(y) * w + x] = SmoothNoise(fx, fy, seed);
        }
    }
}

// --- TileSelectNode ---

std::vector<TilePort> TileSelectNode::Inputs() const {
    return {
        {"Field", TilePinType::Float},
        {"Threshold", TilePinType::Float},
        {"TileBelow", TilePinType::TileID},
        {"TileAbove", TilePinType::TileID}
    };
}

std::vector<TilePort> TileSelectNode::Outputs() const {
    return {{"Map", TilePinType::TileMap}};
}

void TileSelectNode::Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const {
    int fieldSize = ctx.mapWidth * ctx.mapHeight;

    float threshold = 0.5f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        threshold = inputs[1].data[0];
    }

    float tileBelow = 0.0f;
    if (inputs.size() > 2 && !inputs[2].data.empty()) {
        tileBelow = inputs[2].data[0];
    }

    float tileAbove = 1.0f;
    if (inputs.size() > 3 && !inputs[3].data.empty()) {
        tileAbove = inputs[3].data[0];
    }

    outputs.resize(1);
    outputs[0].type = TilePinType::TileMap;
    outputs[0].data.resize(static_cast<size_t>(fieldSize));

    for (int i = 0; i < fieldSize; ++i) {
        float val = 0.0f;
        if (!inputs.empty() && static_cast<size_t>(i) < inputs[0].data.size()) {
            val = inputs[0].data[i];
        }
        outputs[0].data[i] = (val >= threshold) ? tileAbove : tileBelow;
    }
}

// --- BiomeGateNode ---

std::vector<TilePort> BiomeGateNode::Inputs() const {
    return {
        {"Map", TilePinType::TileMap},
        {"Mask", TilePinType::Mask}
    };
}

std::vector<TilePort> BiomeGateNode::Outputs() const {
    return {{"Out", TilePinType::TileMap}};
}

void BiomeGateNode::Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const {
    int fieldSize = ctx.mapWidth * ctx.mapHeight;

    outputs.resize(1);
    outputs[0].type = TilePinType::TileMap;
    outputs[0].data.resize(static_cast<size_t>(fieldSize), 0.0f);

    for (int i = 0; i < fieldSize; ++i) {
        float maskVal = 0.0f;
        if (inputs.size() > 1 && static_cast<size_t>(i) < inputs[1].data.size()) {
            maskVal = inputs[1].data[i];
        }

        if (maskVal > 0.5f) {
            if (!inputs.empty() && static_cast<size_t>(i) < inputs[0].data.size()) {
                outputs[0].data[i] = inputs[0].data[i];
            }
        }
    }
}

}
