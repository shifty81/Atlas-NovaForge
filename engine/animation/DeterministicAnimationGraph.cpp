#include "DeterministicAnimationGraph.h"
#include <algorithm>
#include <queue>

namespace atlas::animation {

BoneNodeID DeterministicAnimationGraph::AddNode(std::unique_ptr<BoneNode> node) {
    BoneNodeID id = m_nextID++;
    m_nodes[id] = std::move(node);
    m_compiled = false;
    return id;
}

void DeterministicAnimationGraph::RemoveNode(BoneNodeID id) {
    m_nodes.erase(id);
    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(),
            [id](const BoneEdge& e) {
                return e.fromNode == id || e.toNode == id;
            }),
        m_edges.end()
    );
    m_compiled = false;
}

void DeterministicAnimationGraph::AddEdge(const BoneEdge& edge) {
    m_edges.push_back(edge);
    m_compiled = false;
}

void DeterministicAnimationGraph::RemoveEdge(const BoneEdge& edge) {
    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(),
            [&edge](const BoneEdge& e) {
                return e.fromNode == edge.fromNode &&
                       e.fromPort == edge.fromPort &&
                       e.toNode == edge.toNode &&
                       e.toPort == edge.toPort;
            }),
        m_edges.end()
    );
    m_compiled = false;
}

bool DeterministicAnimationGraph::HasCycle() const {
    std::unordered_map<BoneNodeID, int> inDegree;
    for (auto& [id, _] : m_nodes) {
        inDegree[id] = 0;
    }
    for (auto& e : m_edges) {
        if (inDegree.count(e.toNode)) {
            inDegree[e.toNode]++;
        }
    }

    std::queue<BoneNodeID> q;
    for (auto& [id, deg] : inDegree) {
        if (deg == 0) q.push(id);
    }

    int visited = 0;
    while (!q.empty()) {
        BoneNodeID n = q.front();
        q.pop();
        visited++;
        for (auto& e : m_edges) {
            if (e.fromNode == n) {
                if (--inDegree[e.toNode] == 0) {
                    q.push(e.toNode);
                }
            }
        }
    }

    return visited != static_cast<int>(m_nodes.size());
}

bool DeterministicAnimationGraph::ValidateEdgeTypes() const {
    for (auto& e : m_edges) {
        auto fromIt = m_nodes.find(e.fromNode);
        auto toIt = m_nodes.find(e.toNode);
        if (fromIt == m_nodes.end() || toIt == m_nodes.end()) return false;

        auto fromOutputs = fromIt->second->Outputs();
        auto toInputs = toIt->second->Inputs();

        if (e.fromPort >= fromOutputs.size()) return false;
        if (e.toPort >= toInputs.size()) return false;

        if (fromOutputs[e.fromPort].type != toInputs[e.toPort].type) return false;
    }
    return true;
}

bool DeterministicAnimationGraph::Compile() {
    m_compiled = false;
    m_executionOrder.clear();

    if (HasCycle()) return false;
    if (!ValidateEdgeTypes()) return false;

    std::unordered_map<BoneNodeID, int> inDegree;
    for (auto& [id, _] : m_nodes) {
        inDegree[id] = 0;
    }
    for (auto& e : m_edges) {
        inDegree[e.toNode]++;
    }

    std::queue<BoneNodeID> q;
    for (auto& [id, deg] : inDegree) {
        if (deg == 0) q.push(id);
    }

    while (!q.empty()) {
        BoneNodeID n = q.front();
        q.pop();
        m_executionOrder.push_back(n);
        for (auto& e : m_edges) {
            if (e.fromNode == n) {
                if (--inDegree[e.toNode] == 0) {
                    q.push(e.toNode);
                }
            }
        }
    }

    m_compiled = (m_executionOrder.size() == m_nodes.size());
    return m_compiled;
}

bool DeterministicAnimationGraph::Execute(const BoneContext& ctx) {
    if (!m_compiled) return false;

    m_outputs.clear();

    for (BoneNodeID id : m_executionOrder) {
        auto it = m_nodes.find(id);
        if (it == m_nodes.end()) return false;

        BoneNode* node = it->second.get();
        auto inputDefs = node->Inputs();

        std::vector<BoneValue> inputs(inputDefs.size());
        for (auto& e : m_edges) {
            if (e.toNode == id && e.toPort < inputs.size()) {
                uint64_t key = (static_cast<uint64_t>(e.fromNode) << 32) | e.fromPort;
                auto outIt = m_outputs.find(key);
                if (outIt != m_outputs.end()) {
                    inputs[e.toPort] = outIt->second;
                }
            }
        }

        auto outputDefs = node->Outputs();
        std::vector<BoneValue> outputs(outputDefs.size());
        node->Evaluate(ctx, inputs, outputs);

        for (BonePortID p = 0; p < outputs.size(); ++p) {
            uint64_t key = (static_cast<uint64_t>(id) << 32) | p;
            m_outputs[key] = std::move(outputs[p]);
        }
    }

    return true;
}

const BoneValue* DeterministicAnimationGraph::GetOutput(BoneNodeID node, BonePortID port) const {
    uint64_t key = (static_cast<uint64_t>(node) << 32) | port;
    auto it = m_outputs.find(key);
    if (it != m_outputs.end()) return &it->second;
    return nullptr;
}

size_t DeterministicAnimationGraph::NodeCount() const {
    return m_nodes.size();
}

bool DeterministicAnimationGraph::IsCompiled() const {
    return m_compiled;
}

// --- Node Implementations ---

void RestPoseNode::Evaluate(const BoneContext& ctx,
                            const std::vector<BoneValue>& /*inputs*/,
                            std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;
    pose.data.resize(ctx.boneCount * 7, 0.0f);
    // Set rotW = 1.0 for each bone (identity quaternion)
    for (uint32_t i = 0; i < ctx.boneCount; ++i) {
        pose.data[i * 7 + 6] = 1.0f; // rotW
    }
    outputs[0] = std::move(pose);
}

void FKNode::Evaluate(const BoneContext& ctx,
                       const std::vector<BoneValue>& inputs,
                       std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        pose.data = inputs[0].data;
    } else {
        pose.data.resize(ctx.boneCount * 7, 0.0f);
        for (uint32_t i = 0; i < ctx.boneCount; ++i)
            pose.data[i * 7 + 6] = 1.0f;
    }
    // Apply forward kinematics: add rotationAngle to rotY for each bone
    uint32_t bones = static_cast<uint32_t>(pose.data.size() / 7);
    for (uint32_t i = 0; i < bones; ++i) {
        pose.data[i * 7 + 4] += rotationAngle; // rotY
    }
    outputs[0] = std::move(pose);
}

void IKNode::Evaluate(const BoneContext& ctx,
                       const std::vector<BoneValue>& inputs,
                       std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        pose.data = inputs[0].data;
    } else {
        pose.data.resize(ctx.boneCount * 7, 0.0f);
        for (uint32_t i = 0; i < ctx.boneCount; ++i)
            pose.data[i * 7 + 6] = 1.0f;
    }

    uint32_t bones = static_cast<uint32_t>(pose.data.size() / 7);
    if (bones == 0) {
        outputs[0] = std::move(pose);
        return;
    }

    // Simple iterative CCD IK: adjust last bones towards target
    for (int iter = 0; iter < iterations; ++iter) {
        for (uint32_t b = bones; b > 0; --b) {
            uint32_t idx = b - 1;
            // End effector is the last bone
            uint32_t endIdx = bones - 1;
            float ex = pose.data[endIdx * 7 + 0];
            float ey = pose.data[endIdx * 7 + 1];
            float ez = pose.data[endIdx * 7 + 2];

            float bx = pose.data[idx * 7 + 0];
            float by = pose.data[idx * 7 + 1];
            float bz = pose.data[idx * 7 + 2];

            // Vector from bone to end effector
            float toEndX = ex - bx, toEndY = ey - by, toEndZ = ez - bz;
            float toEndLen = std::sqrt(toEndX * toEndX + toEndY * toEndY + toEndZ * toEndZ);

            // Vector from bone to target
            float toTgtX = targetX - bx, toTgtY = targetY - by, toTgtZ = targetZ - bz;
            float toTgtLen = std::sqrt(toTgtX * toTgtX + toTgtY * toTgtY + toTgtZ * toTgtZ);

            if (toEndLen > 1e-6f && toTgtLen > 1e-6f) {
                // Compute rotation needed (simplified: adjust position of end effector)
                float scale = toTgtLen / toEndLen;
                if (scale > 2.0f) scale = 2.0f;
                float newEx = bx + toTgtX * (toEndLen / toTgtLen);
                float newEy = by + toTgtY * (toEndLen / toTgtLen);
                float newEz = bz + toTgtZ * (toEndLen / toTgtLen);

                // Move end effector towards target
                float dx = newEx - ex;
                float dy = newEy - ey;
                float dz = newEz - ez;

                // Distribute adjustment to bones from idx to end
                uint32_t affectedCount = endIdx - idx + 1;
                for (uint32_t j = idx; j <= endIdx; ++j) {
                    float t = static_cast<float>(j - idx + 1) / static_cast<float>(affectedCount);
                    pose.data[j * 7 + 0] += dx * t;
                    pose.data[j * 7 + 1] += dy * t;
                    pose.data[j * 7 + 2] += dz * t;
                }
            }
        }
    }

    outputs[0] = std::move(pose);
}

void BlendTreeNode::Evaluate(const BoneContext& ctx,
                              const std::vector<BoneValue>& inputs,
                              std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;

    const auto& poseA = (inputs.size() > 0 && !inputs[0].data.empty()) ? inputs[0].data : std::vector<float>{};
    const auto& poseB = (inputs.size() > 1 && !inputs[1].data.empty()) ? inputs[1].data : std::vector<float>{};

    size_t count = std::max(poseA.size(), poseB.size());
    if (count == 0) count = ctx.boneCount * 7;
    pose.data.resize(count, 0.0f);

    float w = weight;
    for (size_t i = 0; i < count; ++i) {
        float a = (i < poseA.size()) ? poseA[i] : 0.0f;
        float b = (i < poseB.size()) ? poseB[i] : 0.0f;
        pose.data[i] = a * (1.0f - w) + b * w;
    }

    outputs[0] = std::move(pose);
}

void BoneMaskNode::Evaluate(const BoneContext& ctx,
                             const std::vector<BoneValue>& inputs,
                             std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        pose.data = inputs[0].data;
    } else {
        pose.data.resize(ctx.boneCount * 7, 0.0f);
        for (uint32_t i = 0; i < ctx.boneCount; ++i)
            pose.data[i * 7 + 6] = 1.0f;
    }

    uint32_t bones = static_cast<uint32_t>(pose.data.size() / 7);
    for (uint32_t i = 0; i < bones; ++i) {
        bool active = (i < mask.size()) ? mask[i] : false;
        if (!active) {
            for (int c = 0; c < 7; ++c)
                pose.data[i * 7 + c] = 0.0f;
        }
    }

    outputs[0] = std::move(pose);
}

void AdditiveBlendNode::Evaluate(const BoneContext& ctx,
                                  const std::vector<BoneValue>& inputs,
                                  std::vector<BoneValue>& outputs) const {
    BoneValue pose;
    pose.type = BonePinType::BoneTransform;

    const auto& base = (inputs.size() > 0 && !inputs[0].data.empty()) ? inputs[0].data : std::vector<float>{};
    const auto& additive = (inputs.size() > 1 && !inputs[1].data.empty()) ? inputs[1].data : std::vector<float>{};

    size_t count = std::max(base.size(), additive.size());
    if (count == 0) count = ctx.boneCount * 7;
    pose.data.resize(count, 0.0f);

    for (size_t i = 0; i < count; ++i) {
        float b = (i < base.size()) ? base[i] : 0.0f;
        float a = (i < additive.size()) ? additive[i] : 0.0f;
        pose.data[i] = b + a * strength;
    }

    outputs[0] = std::move(pose);
}

} // namespace atlas::animation
