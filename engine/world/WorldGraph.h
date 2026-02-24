#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace atlas::world {

enum class ValueType : uint8_t {
    Float,
    Int,
    Vec2,
    Vec3,
    Bool,
    Seed,
    HeightField,
    DensityField,
    Mask
};

struct Value {
    ValueType type;
    std::vector<float> data;
};

struct NodePort {
    std::string name;
    ValueType type;
};

using NodeID = uint32_t;
using PortID = uint16_t;

struct Edge {
    NodeID fromNode;
    PortID fromPort;
    NodeID toNode;
    PortID toPort;
};

struct WorldGenContext {
    uint64_t worldSeed;
    int32_t lod;
    int32_t chunkX;
    int32_t chunkY;
    int32_t chunkZ;
};

class WorldNode {
public:
    virtual ~WorldNode() = default;

    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;
    virtual std::vector<NodePort> Inputs() const = 0;
    virtual std::vector<NodePort> Outputs() const = 0;

    virtual void Evaluate(
        const WorldGenContext& ctx,
        const std::vector<Value>& inputs,
        std::vector<Value>& outputs
    ) const = 0;
};

class WorldGraph {
public:
    NodeID AddNode(std::unique_ptr<WorldNode> node);
    void RemoveNode(NodeID id);
    void AddEdge(const Edge& edge);
    void RemoveEdge(const Edge& edge);

    // Topological sort + validation, returns true on success
    bool Compile();

    // Execute compiled graph
    bool Execute(const WorldGenContext& ctx);

    const Value* GetOutput(NodeID node, PortID port) const;
    size_t NodeCount() const;
    bool IsCompiled() const;

    // Node access for editor/inspection
    std::vector<NodeID> GetNodeIDs() const;
    const WorldNode* GetNode(NodeID id) const;

private:
    NodeID m_nextID = 1;
    std::unordered_map<NodeID, std::unique_ptr<WorldNode>> m_nodes;
    std::vector<Edge> m_edges;
    std::vector<NodeID> m_executionOrder;
    bool m_compiled = false;

    // Per-node output storage keyed by (nodeID << 32 | port)
    std::unordered_map<uint64_t, Value> m_outputs;

    bool HasCycle() const;
    bool ValidateEdgeTypes() const;
};

}
