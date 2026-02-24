#include "FlowGraphDebugger.h"

namespace atlas::flow {

// --- Breakpoints ---

void FlowGraphDebugger::AddBreakpoint(FlowNodeID nodeID) {
    m_breakpoints.insert(nodeID);
}

void FlowGraphDebugger::RemoveBreakpoint(FlowNodeID nodeID) {
    m_breakpoints.erase(nodeID);
}

bool FlowGraphDebugger::HasBreakpoint(FlowNodeID nodeID) const {
    return m_breakpoints.count(nodeID) > 0;
}

void FlowGraphDebugger::ClearBreakpoints() {
    m_breakpoints.clear();
}

const std::set<FlowNodeID>& FlowGraphDebugger::Breakpoints() const {
    return m_breakpoints;
}

// --- Execution control ---

void FlowGraphDebugger::SetAction(DebugAction action) {
    m_action = action;
    if (action != DebugAction::Pause) {
        m_paused = false;
    }
}

DebugAction FlowGraphDebugger::CurrentAction() const {
    return m_action;
}

bool FlowGraphDebugger::IsPaused() const {
    return m_paused;
}

// --- Execution with debugging ---

bool FlowGraphDebugger::ExecuteDebug(GameFlowGraph& graph, const FlowContext& ctx) {
    if (!graph.IsCompiled()) return false;

    const auto& order = graph.ExecutionOrder();
    const auto& edges = graph.Edges();

    m_nodeStates.clear();
    m_nodeStates.reserve(order.size());

    for (FlowNodeID id : order) {
        FlowNode* node = graph.GetNode(id);
        if (!node) return false;

        m_currentNode = id;

        // Gather inputs from upstream edges
        auto inputDefs = node->Inputs();
        std::vector<FlowValue> inputs(inputDefs.size());
        for (const auto& e : edges) {
            if (e.toNode == id && e.toPort < inputs.size()) {
                const FlowValue* out = graph.GetOutput(e.fromNode, e.fromPort);
                if (out) {
                    inputs[e.toPort] = *out;
                }
            }
        }

        // Evaluate
        auto outputDefs = node->Outputs();
        std::vector<FlowValue> outputs(outputDefs.size());
        node->Evaluate(ctx, inputs, outputs);

        // Store outputs in graph
        for (FlowPortID p = 0; p < static_cast<FlowPortID>(outputs.size()); ++p) {
            graph.SetOutput(id, p, outputs[p]);
        }

        // Record state
        DebugNodeState state;
        state.nodeID = id;
        state.inputs = inputs;
        state.outputs = outputs;
        state.wasExecuted = true;
        m_nodeStates.push_back(std::move(state));

        ++m_executionStep;

        // Check breakpoint
        if (HasBreakpoint(id)) {
            m_paused = true;
            m_action = DebugAction::Pause;
            if (m_callback) {
                m_callback(id, m_nodeStates.back());
            }
            return true;
        }

        // StepNode: execute one node then pause
        if (m_action == DebugAction::StepNode) {
            m_paused = true;
            m_action = DebugAction::Pause;
            return true;
        }
    }

    return true;
}

// --- Value inspection ---

const std::vector<DebugNodeState>& FlowGraphDebugger::NodeStates() const {
    return m_nodeStates;
}

const DebugNodeState* FlowGraphDebugger::GetNodeState(FlowNodeID nodeID) const {
    for (const auto& s : m_nodeStates) {
        if (s.nodeID == nodeID) return &s;
    }
    return nullptr;
}

// --- Timeline ---

size_t FlowGraphDebugger::ExecutionStep() const {
    return m_executionStep;
}

FlowNodeID FlowGraphDebugger::CurrentNode() const {
    return m_currentNode;
}

// --- Callback ---

void FlowGraphDebugger::SetBreakpointCallback(BreakpointCallback cb) {
    m_callback = std::move(cb);
}

} // namespace atlas::flow
