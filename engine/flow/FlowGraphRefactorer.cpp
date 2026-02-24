#include "FlowGraphRefactorer.h"
#include <algorithm>

namespace atlas::flow {

void FlowGraphRefactorer::LoadGraph(const std::vector<FlowNodeInfo>& nodes) {
    m_nodes.clear();
    for (const auto& n : nodes) {
        m_nodes[n.id] = n;
    }
}

size_t FlowGraphRefactorer::NodeCount() const {
    return m_nodes.size();
}

const FlowNodeInfo* FlowGraphRefactorer::GetNode(uint32_t id) const {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) return nullptr;
    return &it->second;
}

std::vector<uint32_t> FlowGraphRefactorer::FindDeadNodes() const {
    // Collect all nodes referenced by other nodes (via inputEdges or outputEdges)
    std::unordered_set<uint32_t> referenced;
    for (const auto& kv : m_nodes) {
        for (uint32_t src : kv.second.inputEdges) {
            referenced.insert(src);
        }
        for (uint32_t dst : kv.second.outputEdges) {
            referenced.insert(dst);
        }
    }

    std::vector<uint32_t> dead;
    for (const auto& kv : m_nodes) {
        const auto& node = kv.second;
        // A dead node has no connections at all
        if (node.outputEdges.empty() && node.inputEdges.empty() &&
            referenced.find(node.id) == referenced.end()) {
            dead.push_back(node.id);
        }
    }
    std::sort(dead.begin(), dead.end());
    return dead;
}

std::vector<std::vector<uint32_t>> FlowGraphRefactorer::FindLinearChains() const {
    // Find chain start nodes: nodes that have exactly 1 output, where
    // either they have != 1 input, or their single input node doesn't
    // have exactly 1 output (so they are true chain starts).
    std::vector<std::vector<uint32_t>> chains;

    std::unordered_set<uint32_t> inChain;

    // First, find all potential chain start nodes
    std::vector<uint32_t> starts;
    for (const auto& kv : m_nodes) {
        const auto& node = kv.second;
        if (node.outputEdges.size() != 1) continue;

        // Check if this is a chain start: no single-output predecessor
        bool isStart = true;
        if (node.inputEdges.size() == 1) {
            auto predIt = m_nodes.find(node.inputEdges[0]);
            if (predIt != m_nodes.end() && predIt->second.outputEdges.size() == 1) {
                isStart = false; // predecessor also chains into us
            }
        }
        if (isStart) starts.push_back(node.id);
    }

    for (uint32_t startID : starts) {
        if (inChain.count(startID)) continue;

        std::vector<uint32_t> chain;
        chain.push_back(startID);

        uint32_t current = startID;
        while (true) {
            auto it = m_nodes.find(current);
            if (it == m_nodes.end()) break;
            if (it->second.outputEdges.size() != 1) break;

            uint32_t nextID = it->second.outputEdges[0];
            auto nextIt = m_nodes.find(nextID);
            if (nextIt == m_nodes.end()) break;
            if (nextIt->second.inputEdges.size() != 1) break;

            chain.push_back(nextID);
            current = nextID;
        }

        if (chain.size() >= 2) {
            for (uint32_t id : chain) inChain.insert(id);
            chains.push_back(chain);
        }
    }

    return chains;
}

std::vector<uint32_t> FlowGraphRefactorer::FindDuplicatePatterns() const {
    // Find nodes with the same name and category
    std::unordered_map<std::string, std::vector<uint32_t>> patterns;
    for (const auto& kv : m_nodes) {
        std::string key = kv.second.name + "|" + kv.second.category;
        patterns[key].push_back(kv.first);
    }

    std::vector<uint32_t> duplicates;
    for (const auto& kv : patterns) {
        if (kv.second.size() > 1) {
            for (uint32_t id : kv.second) {
                duplicates.push_back(id);
            }
        }
    }
    std::sort(duplicates.begin(), duplicates.end());
    return duplicates;
}

std::vector<RefactorSuggestion> FlowGraphRefactorer::Analyze() const {
    std::vector<RefactorSuggestion> suggestions;

    auto dead = FindDeadNodes();
    if (!dead.empty()) {
        RefactorSuggestion s;
        s.type = RefactorType::RemoveDeadNodes;
        s.description = "Remove " + std::to_string(dead.size()) + " dead node(s)";
        s.affectedNodes = dead;
        s.impact = 0.3;
        suggestions.push_back(s);
    }

    auto chains = FindLinearChains();
    for (const auto& chain : chains) {
        RefactorSuggestion s;
        s.type = RefactorType::SimplifyChain;
        s.description = "Simplify linear chain of " + std::to_string(chain.size()) + " node(s)";
        s.affectedNodes = chain;
        s.impact = 0.5;
        suggestions.push_back(s);
    }

    auto dups = FindDuplicatePatterns();
    if (!dups.empty()) {
        RefactorSuggestion s;
        s.type = RefactorType::ExtractSubgraph;
        s.description = "Extract " + std::to_string(dups.size()) + " duplicate pattern node(s)";
        s.affectedNodes = dups;
        s.impact = 0.4;
        suggestions.push_back(s);
    }

    return suggestions;
}

RefactorResult FlowGraphRefactorer::RemoveDeadNodes() {
    auto dead = FindDeadNodes();
    RefactorResult result;
    result.success = true;
    result.nodesRemoved = dead.size();
    result.description = "Removed " + std::to_string(dead.size()) + " dead node(s)";

    for (uint32_t id : dead) {
        m_nodes.erase(id);
    }

    m_history.push_back(result);
    return result;
}

RefactorResult FlowGraphRefactorer::RenameNode(uint32_t nodeID, const std::string& newName) {
    RefactorResult result;
    auto it = m_nodes.find(nodeID);
    if (it == m_nodes.end()) {
        result.success = false;
        result.description = "Node not found: " + std::to_string(nodeID);
        m_history.push_back(result);
        return result;
    }

    result.success = true;
    result.description = "Renamed node " + std::to_string(nodeID) + " from '" +
                         it->second.name + "' to '" + newName + "'";
    it->second.name = newName;
    m_history.push_back(result);
    return result;
}

RefactorResult FlowGraphRefactorer::SimplifyChain(const std::vector<uint32_t>& chain) {
    RefactorResult result;
    if (chain.size() < 2) {
        result.success = false;
        result.description = "Chain too short to simplify";
        m_history.push_back(result);
        return result;
    }

    result.success = true;
    result.description = "Simplified chain of " + std::to_string(chain.size()) + " node(s)";
    result.nodesRemoved = chain.size() - 1;
    result.edgesModified = chain.size() - 1;

    // Keep the first node, remove the rest, and update edges
    auto firstIt = m_nodes.find(chain[0]);
    auto lastIt = m_nodes.find(chain.back());
    if (firstIt != m_nodes.end() && lastIt != m_nodes.end()) {
        firstIt->second.outputEdges = lastIt->second.outputEdges;
    }

    for (size_t i = 1; i < chain.size(); ++i) {
        m_nodes.erase(chain[i]);
    }

    m_history.push_back(result);
    return result;
}

const std::vector<RefactorResult>& FlowGraphRefactorer::RefactorHistory() const {
    return m_history;
}

void FlowGraphRefactorer::Clear() {
    m_nodes.clear();
    m_history.clear();
}

} // namespace atlas::flow
