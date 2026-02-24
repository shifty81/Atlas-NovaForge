#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace atlas::flow {

enum class FlowPinType : uint8_t {
    Trigger,
    State,
    Bool,
    Float,
    String
};

struct FlowValue {
    FlowPinType type;
    std::vector<float> data;
    std::string text;
};

struct FlowPort {
    std::string name;
    FlowPinType type;
};

using FlowNodeID = uint32_t;
using FlowPortID = uint16_t;

struct FlowEdge {
    FlowNodeID fromNode;
    FlowPortID fromPort;
    FlowNodeID toNode;
    FlowPortID toPort;
};

struct FlowContext {
    float elapsedTime;
    bool inputReceived;
    uint32_t tick;
};

class FlowNode {
public:
    virtual ~FlowNode() = default;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;
    virtual std::vector<FlowPort> Inputs() const = 0;
    virtual std::vector<FlowPort> Outputs() const = 0;
    virtual void Evaluate(const FlowContext& ctx,
                          const std::vector<FlowValue>& inputs,
                          std::vector<FlowValue>& outputs) const = 0;
};

class GameFlowGraph {
public:
    FlowNodeID AddNode(std::unique_ptr<FlowNode> node);
    void RemoveNode(FlowNodeID id);
    void AddEdge(const FlowEdge& edge);
    void RemoveEdge(const FlowEdge& edge);

    bool Compile();
    bool Execute(const FlowContext& ctx);

    const FlowValue* GetOutput(FlowNodeID node, FlowPortID port) const;
    size_t NodeCount() const;
    bool IsCompiled() const;

    // Accessors for debugger / IR tooling
    const std::vector<FlowNodeID>& ExecutionOrder() const;
    FlowNode* GetNode(FlowNodeID id) const;
    const std::vector<FlowEdge>& Edges() const;
    void SetOutput(FlowNodeID node, FlowPortID port, FlowValue value);

private:
    FlowNodeID m_nextID = 1;
    std::unordered_map<FlowNodeID, std::unique_ptr<FlowNode>> m_nodes;
    std::vector<FlowEdge> m_edges;
    std::vector<FlowNodeID> m_executionOrder;
    bool m_compiled = false;

    std::unordered_map<uint64_t, FlowValue> m_outputs;

    bool HasCycle() const;
    bool ValidateEdgeTypes() const;
};

}
