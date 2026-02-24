#pragma once
#include "WorldGraph.h"

namespace atlas::world {

// Outputs the world seed from context
class SeedNode : public WorldNode {
public:
    const char* GetName() const override { return "Seed"; }
    const char* GetCategory() const override { return "Input"; }
    std::vector<NodePort> Inputs() const override { return {}; }
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// FBM noise generation using existing NoiseGenerator
class NoiseNode : public WorldNode {
public:
    const char* GetName() const override { return "Noise"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<NodePort> Inputs() const override;
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// Blend two heightfields with a factor
class BlendNode : public WorldNode {
public:
    const char* GetName() const override { return "Blend"; }
    const char* GetCategory() const override { return "Filter"; }
    std::vector<NodePort> Inputs() const override;
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// Clamp heightfield values
class ClampNode : public WorldNode {
public:
    const char* GetName() const override { return "Clamp"; }
    const char* GetCategory() const override { return "Filter"; }
    std::vector<NodePort> Inputs() const override;
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// Constant float output
class ConstantNode : public WorldNode {
public:
    float value = 0.0f;

    const char* GetName() const override { return "Constant"; }
    const char* GetCategory() const override { return "Input"; }
    std::vector<NodePort> Inputs() const override { return {}; }
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// Classifies heightfield cells into biome types based on elevation and moisture
// Biome IDs: 0=Ocean, 1=Beach, 2=Plains, 3=Forest, 4=Mountain, 5=Snow
class BiomeNode : public WorldNode {
public:
    const char* GetName() const override { return "Biome"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<NodePort> Inputs() const override;
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

// Applies hydraulic erosion simulation to a heightfield
class ErosionNode : public WorldNode {
public:
    int iterations = 50;

    const char* GetName() const override { return "Erosion"; }
    const char* GetCategory() const override { return "Filter"; }
    std::vector<NodePort> Inputs() const override;
    std::vector<NodePort> Outputs() const override;
    void Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const override;
};

}
