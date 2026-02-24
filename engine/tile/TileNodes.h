#pragma once
#include "TileGraph.h"

namespace atlas::tile {

// Outputs a constant TileID
class BaseTileNode : public TileNode {
public:
    float tileID = 0.0f; // 0=grass, 1=stone, 2=metal, etc.

    const char* GetName() const override { return "BaseTile"; }
    const char* GetCategory() const override { return "Input"; }
    std::vector<TilePort> Inputs() const override { return {}; }
    std::vector<TilePort> Outputs() const override;
    void Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const override;
};

// Generates a noise-based float field from seed + frequency
class NoiseFieldNode : public TileNode {
public:
    const char* GetName() const override { return "NoiseField"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<TilePort> Inputs() const override;
    std::vector<TilePort> Outputs() const override;
    void Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const override;
};

// Takes a float field and thresholds to select tile IDs
class TileSelectNode : public TileNode {
public:
    const char* GetName() const override { return "TileSelect"; }
    const char* GetCategory() const override { return "Selector"; }
    std::vector<TilePort> Inputs() const override;
    std::vector<TilePort> Outputs() const override;
    void Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const override;
};

// Takes a float field mask and outputs filtered TileMap
class BiomeGateNode : public TileNode {
public:
    const char* GetName() const override { return "BiomeGate"; }
    const char* GetCategory() const override { return "Filter"; }
    std::vector<TilePort> Inputs() const override;
    std::vector<TilePort> Outputs() const override;
    void Evaluate(const TileGenContext& ctx, const std::vector<TileValue>& inputs, std::vector<TileValue>& outputs) const override;
};

}
