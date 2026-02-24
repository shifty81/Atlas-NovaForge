#pragma once
#include "CharacterGraph.h"

namespace atlas::character {

// Generates body proportions from seed (height, mass, proportions as floats)
class BaseBodyNode : public CharacterNode {
public:
    const char* GetName() const override { return "BaseBody"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

// Produces skeleton data from height
class SkeletonNode : public CharacterNode {
public:
    const char* GetName() const override { return "Skeleton"; }
    const char* GetCategory() const override { return "Rigging"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

// Generates appearance from seed + faction
class MaterialNode : public CharacterNode {
public:
    const char* GetName() const override { return "Material"; }
    const char* GetCategory() const override { return "Appearance"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

// Attaches equipment to mesh
class EquipmentNode : public CharacterNode {
public:
    const char* GetName() const override { return "Equipment"; }
    const char* GetCategory() const override { return "Attachment"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

// Generates procedural facial features from seed (nose width, eye spacing, jaw width, brow height, lip fullness)
class FacialFeatureNode : public CharacterNode {
public:
    const char* GetName() const override { return "FacialFeature"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

// Generates procedural hair style and color from seed (style index, length, density, R, G, B)
class HairStyleNode : public CharacterNode {
public:
    const char* GetName() const override { return "HairStyle"; }
    const char* GetCategory() const override { return "Appearance"; }
    std::vector<CharacterPort> Inputs() const override;
    std::vector<CharacterPort> Outputs() const override;
    void Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const override;
};

}
