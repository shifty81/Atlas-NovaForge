#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "GameFlowGraph.h"

namespace atlas::flow {

struct FlowNodeIR {
    uint32_t id = 0;
    std::string type;       // e.g., "State", "Condition", "Timer", "Transition"
    std::string category;   // e.g., "Flow", "Logic", "Control"
    float posX = 0.0f;      // Editor position X
    float posY = 0.0f;      // Editor position Y
    std::vector<std::pair<std::string, std::string>> properties; // key-value pairs
};

struct FlowGraphIR {
    static constexpr uint32_t CURRENT_VERSION = 1;

    uint32_t version = CURRENT_VERSION;
    std::string name;
    std::string graphType;  // "Gameplay", "UI", "AI"
    std::vector<FlowNodeIR> nodes;
    std::vector<FlowEdge> edges;

    // Serialization
    std::string ToJSON() const;
    static FlowGraphIR FromJSON(const std::string& json);

    // Validation
    bool Validate() const;

    // Migration
    static FlowGraphIR Migrate(const FlowGraphIR& old, uint32_t targetVersion);
};

} // namespace atlas::flow
