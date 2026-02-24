#include "CollaborativeEditor.h"
#include <algorithm>

namespace atlas::graphvm {

void CollaborativeEditor::AddPeer(uint32_t peerID, const std::string& name) {
    CollaboratorInfo info;
    info.peerID = peerID;
    info.name = name;
    info.isActive = true;
    m_peers[peerID] = std::move(info);
}

void CollaborativeEditor::RemovePeer(uint32_t peerID) {
    m_peers.erase(peerID);
}

const CollaboratorInfo* CollaborativeEditor::GetPeer(uint32_t peerID) const {
    auto it = m_peers.find(peerID);
    if (it != m_peers.end()) return &it->second;
    return nullptr;
}

std::vector<CollaboratorInfo> CollaborativeEditor::ActivePeers() const {
    std::vector<CollaboratorInfo> result;
    for (auto& [id, info] : m_peers) {
        if (info.isActive) result.push_back(info);
    }
    return result;
}

size_t CollaborativeEditor::PeerCount() const {
    return m_peers.size();
}

void CollaborativeEditor::UpdateCursor(uint32_t peerID, const CursorPosition& pos) {
    auto it = m_peers.find(peerID);
    if (it != m_peers.end()) {
        it->second.cursor = pos;
    }
}

CursorPosition CollaborativeEditor::GetCursor(uint32_t peerID) const {
    auto it = m_peers.find(peerID);
    if (it != m_peers.end()) return it->second.cursor;
    return {};
}

void CollaborativeEditor::SubmitOperation(const EditOperation& op) {
    EditOperation local = op;
    local.sequenceNumber = m_nextSeq++;
    m_localOps.push_back(local);
    m_opLog.push_back(local);
}

void CollaborativeEditor::ReceiveRemoteOperation(const EditOperation& op) {
    EditOperation remote = op;
    if (remote.sequenceNumber == 0) {
        remote.sequenceNumber = m_nextSeq++;
    }
    m_remoteOps.push_back(remote);
    m_opLog.push_back(remote);
}

const std::vector<EditOperation>& CollaborativeEditor::OperationLog() const {
    return m_opLog;
}

size_t CollaborativeEditor::OperationCount() const {
    return m_opLog.size();
}

std::vector<ConflictReport> CollaborativeEditor::DetectConflicts() const {
    std::vector<ConflictReport> conflicts;

    for (auto& local : m_localOps) {
        for (auto& remote : m_remoteOps) {
            if (local.targetNodeID != remote.targetNodeID) continue;

            ConflictReport report;
            report.localOp = local;
            report.remoteOp = remote;

            // Delete vs Modify conflict
            if ((local.type == EditOpType::RemoveNode && remote.type == EditOpType::ModifyProperty) ||
                (local.type == EditOpType::ModifyProperty && remote.type == EditOpType::RemoveNode)) {
                report.type = ConflictType::DeleteModify;
                report.description = "Delete conflicts with modify on node " + std::to_string(local.targetNodeID);
                conflicts.push_back(report);
            }
            // Concurrent modify
            else if (local.type == EditOpType::ModifyProperty && remote.type == EditOpType::ModifyProperty) {
                report.type = ConflictType::ConcurrentModify;
                report.description = "Concurrent modify on node " + std::to_string(local.targetNodeID);
                conflicts.push_back(report);
            }
            // Move conflict
            else if (local.type == EditOpType::MoveNode && remote.type == EditOpType::MoveNode) {
                report.type = ConflictType::MoveConflict;
                report.description = "Concurrent move on node " + std::to_string(local.targetNodeID);
                conflicts.push_back(report);
            }
        }
    }

    return conflicts;
}

size_t CollaborativeEditor::ConflictCount() const {
    return DetectConflicts().size();
}

void CollaborativeEditor::SetResolutionStrategy(ResolutionStrategy strategy) {
    m_strategy = strategy;
}

ResolutionStrategy CollaborativeEditor::GetResolutionStrategy() const {
    return m_strategy;
}

std::vector<EditOperation> CollaborativeEditor::ResolveConflicts() {
    auto conflicts = DetectConflicts();
    std::vector<EditOperation> resolved;

    if (m_strategy == ResolutionStrategy::ManualResolve) {
        return resolved; // empty = needs manual resolution
    }

    for (auto& conflict : conflicts) {
        if (m_strategy == ResolutionStrategy::LastWriterWins) {
            if (conflict.localOp.timestamp >= conflict.remoteOp.timestamp) {
                resolved.push_back(conflict.localOp);
            } else {
                resolved.push_back(conflict.remoteOp);
            }
        } else if (m_strategy == ResolutionStrategy::FirstWriterWins) {
            if (conflict.localOp.timestamp <= conflict.remoteOp.timestamp) {
                resolved.push_back(conflict.localOp);
            } else {
                resolved.push_back(conflict.remoteOp);
            }
        }
    }

    // Clear conflict sources after resolution
    m_localOps.clear();
    m_remoteOps.clear();

    return resolved;
}

void CollaborativeEditor::Clear() {
    m_peers.clear();
    m_localOps.clear();
    m_remoteOps.clear();
    m_opLog.clear();
    m_nextSeq = 1;
}

} // namespace atlas::graphvm
