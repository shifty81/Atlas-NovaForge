#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cmath>

namespace atlas::animation {

enum class BonePinType : uint8_t {
    Float,
    BoneTransform,   // Position + rotation per bone
    IKTarget,        // IK target position
    BlendWeight,     // Blend tree weight
    BoneMask         // Which bones are affected
};

struct BoneTransform {
    float posX = 0, posY = 0, posZ = 0;
    float rotX = 0, rotY = 0, rotZ = 0, rotW = 1.0f;
};

struct BoneValue {
    BonePinType type;
    std::vector<float> data;
};

struct BonePort {
    std::string name;
    BonePinType type;
};

using BoneNodeID = uint32_t;
using BonePortID = uint16_t;

struct BoneEdge {
    BoneNodeID fromNode;
    BonePortID fromPort;
    BoneNodeID toNode;
    BonePortID toPort;
};

struct BoneContext {
    float deltaTime = 0.016f;
    uint32_t tick = 0;
    uint32_t boneCount = 8;
    uint64_t seed = 0;
};

class BoneNode {
public:
    virtual ~BoneNode() = default;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;
    virtual std::vector<BonePort> Inputs() const = 0;
    virtual std::vector<BonePort> Outputs() const = 0;
    virtual void Evaluate(const BoneContext& ctx,
                          const std::vector<BoneValue>& inputs,
                          std::vector<BoneValue>& outputs) const = 0;
};

class DeterministicAnimationGraph {
public:
    BoneNodeID AddNode(std::unique_ptr<BoneNode> node);
    void RemoveNode(BoneNodeID id);
    void AddEdge(const BoneEdge& edge);
    void RemoveEdge(const BoneEdge& edge);

    bool Compile();
    bool Execute(const BoneContext& ctx);

    const BoneValue* GetOutput(BoneNodeID node, BonePortID port) const;
    size_t NodeCount() const;
    bool IsCompiled() const;

private:
    BoneNodeID m_nextID = 1;
    std::unordered_map<BoneNodeID, std::unique_ptr<BoneNode>> m_nodes;
    std::vector<BoneEdge> m_edges;
    std::vector<BoneNodeID> m_executionOrder;
    bool m_compiled = false;
    std::unordered_map<uint64_t, BoneValue> m_outputs;

    bool HasCycle() const;
    bool ValidateEdgeTypes() const;
};

// --- Node Types ---

class RestPoseNode : public BoneNode {
public:
    const char* GetName() const override { return "RestPose"; }
    const char* GetCategory() const override { return "Source"; }
    std::vector<BonePort> Inputs() const override { return {}; }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

class FKNode : public BoneNode {
public:
    float rotationAngle = 0.1f;
    const char* GetName() const override { return "FK"; }
    const char* GetCategory() const override { return "Kinematics"; }
    std::vector<BonePort> Inputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

class IKNode : public BoneNode {
public:
    float targetX = 1.0f, targetY = 1.0f, targetZ = 0.0f;
    int iterations = 4;
    const char* GetName() const override { return "IK"; }
    const char* GetCategory() const override { return "Kinematics"; }
    std::vector<BonePort> Inputs() const override {
        return {{"Pose", BonePinType::BoneTransform}, {"Target", BonePinType::IKTarget}};
    }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

class BlendTreeNode : public BoneNode {
public:
    float weight = 0.5f;
    const char* GetName() const override { return "BlendTree"; }
    const char* GetCategory() const override { return "Blend"; }
    std::vector<BonePort> Inputs() const override {
        return {{"PoseA", BonePinType::BoneTransform}, {"PoseB", BonePinType::BoneTransform}};
    }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

class BoneMaskNode : public BoneNode {
public:
    std::vector<bool> mask;  // true = active
    const char* GetName() const override { return "BoneMask"; }
    const char* GetCategory() const override { return "Filter"; }
    std::vector<BonePort> Inputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

class AdditiveBlendNode : public BoneNode {
public:
    float strength = 1.0f;
    const char* GetName() const override { return "AdditiveBlend"; }
    const char* GetCategory() const override { return "Blend"; }
    std::vector<BonePort> Inputs() const override {
        return {{"Base", BonePinType::BoneTransform}, {"Additive", BonePinType::BoneTransform}};
    }
    std::vector<BonePort> Outputs() const override {
        return {{"Pose", BonePinType::BoneTransform}};
    }
    void Evaluate(const BoneContext& ctx, const std::vector<BoneValue>& inputs, std::vector<BoneValue>& outputs) const override;
};

} // namespace atlas::animation
