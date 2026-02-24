#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace atlas::character {

enum class CharacterPinType : uint8_t {
    Float,
    Mesh,
    Material,
    Skeleton,
    Seed,
    Equipment
};

struct CharacterValue {
    CharacterPinType type;
    std::vector<float> data;
};

struct CharacterPort {
    std::string name;
    CharacterPinType type;
};

using CharacterNodeID = uint32_t;
using CharacterPortID = uint16_t;

struct CharacterEdge {
    CharacterNodeID fromNode;
    CharacterPortID fromPort;
    CharacterNodeID toNode;
    CharacterPortID toPort;
};

struct CharacterContext {
    uint64_t seed;
    uint32_t factionID;
    float age;
    float fitness;
};

class CharacterNode {
public:
    virtual ~CharacterNode() = default;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;
    virtual std::vector<CharacterPort> Inputs() const = 0;
    virtual std::vector<CharacterPort> Outputs() const = 0;
    virtual void Evaluate(const CharacterContext& ctx,
                          const std::vector<CharacterValue>& inputs,
                          std::vector<CharacterValue>& outputs) const = 0;
};

class CharacterGraph {
public:
    CharacterNodeID AddNode(std::unique_ptr<CharacterNode> node);
    void RemoveNode(CharacterNodeID id);
    void AddEdge(const CharacterEdge& edge);
    void RemoveEdge(const CharacterEdge& edge);

    bool Compile();
    bool Execute(const CharacterContext& ctx);

    const CharacterValue* GetOutput(CharacterNodeID node, CharacterPortID port) const;
    size_t NodeCount() const;
    bool IsCompiled() const;

private:
    CharacterNodeID m_nextID = 1;
    std::unordered_map<CharacterNodeID, std::unique_ptr<CharacterNode>> m_nodes;
    std::vector<CharacterEdge> m_edges;
    std::vector<CharacterNodeID> m_executionOrder;
    bool m_compiled = false;

    std::unordered_map<uint64_t, CharacterValue> m_outputs;

    bool HasCycle() const;
    bool ValidateEdgeTypes() const;
};

}
