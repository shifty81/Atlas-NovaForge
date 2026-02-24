#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace atlas::flow {

enum class RefactorType : uint8_t {
    ExtractSubgraph,
    InlineSubgraph,
    RenameNode,
    RemoveDeadNodes,
    SimplifyChain,
    ReorderNodes
};

struct RefactorSuggestion {
    RefactorType type;
    std::string description;
    std::vector<uint32_t> affectedNodes;
    double impact = 0.0;
};

struct RefactorResult {
    bool success = false;
    std::string description;
    size_t nodesAdded = 0;
    size_t nodesRemoved = 0;
    size_t edgesModified = 0;
};

struct FlowNodeInfo {
    uint32_t id = 0;
    std::string name;
    std::string category;
    std::vector<uint32_t> inputEdges;
    std::vector<uint32_t> outputEdges;
};

class FlowGraphRefactorer {
public:
    void LoadGraph(const std::vector<FlowNodeInfo>& nodes);
    size_t NodeCount() const;
    const FlowNodeInfo* GetNode(uint32_t id) const;

    std::vector<RefactorSuggestion> Analyze() const;
    std::vector<uint32_t> FindDeadNodes() const;
    std::vector<std::vector<uint32_t>> FindLinearChains() const;
    std::vector<uint32_t> FindDuplicatePatterns() const;

    RefactorResult RemoveDeadNodes();
    RefactorResult RenameNode(uint32_t nodeID, const std::string& newName);
    RefactorResult SimplifyChain(const std::vector<uint32_t>& chain);

    const std::vector<RefactorResult>& RefactorHistory() const;

    void Clear();

private:
    std::unordered_map<uint32_t, FlowNodeInfo> m_nodes;
    std::vector<RefactorResult> m_history;
};

} // namespace atlas::flow
