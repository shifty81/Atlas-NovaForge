#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace atlas::graphvm {

struct CursorPosition {
    uint32_t nodeID = 0;
    float x = 0, y = 0;
    uint64_t timestamp = 0;
};

struct CollaboratorInfo {
    uint32_t peerID = 0;
    std::string name;
    CursorPosition cursor;
    bool isActive = true;
};

enum class EditOpType : uint8_t {
    AddNode,
    RemoveNode,
    MoveNode,
    AddEdge,
    RemoveEdge,
    ModifyProperty
};

struct EditOperation {
    uint32_t peerID = 0;
    EditOpType type;
    uint32_t targetNodeID = 0;
    uint32_t secondaryID = 0;   // For edge ops
    std::string propertyName;
    std::string propertyValue;
    uint64_t timestamp = 0;
    uint64_t sequenceNumber = 0;
};

enum class ConflictType : uint8_t {
    None,
    ConcurrentModify,
    DeleteModify,
    MoveConflict
};

struct ConflictReport {
    ConflictType type = ConflictType::None;
    EditOperation localOp;
    EditOperation remoteOp;
    std::string description;
};

enum class ResolutionStrategy : uint8_t {
    LastWriterWins,
    FirstWriterWins,
    ManualResolve
};

class CollaborativeEditor {
public:
    // Peer management
    void AddPeer(uint32_t peerID, const std::string& name);
    void RemovePeer(uint32_t peerID);
    const CollaboratorInfo* GetPeer(uint32_t peerID) const;
    std::vector<CollaboratorInfo> ActivePeers() const;
    size_t PeerCount() const;

    // Cursor tracking
    void UpdateCursor(uint32_t peerID, const CursorPosition& pos);
    CursorPosition GetCursor(uint32_t peerID) const;

    // Operations
    void SubmitOperation(const EditOperation& op);
    void ReceiveRemoteOperation(const EditOperation& op);
    const std::vector<EditOperation>& OperationLog() const;
    size_t OperationCount() const;

    // Conflict detection & resolution
    std::vector<ConflictReport> DetectConflicts() const;
    size_t ConflictCount() const;
    void SetResolutionStrategy(ResolutionStrategy strategy);
    ResolutionStrategy GetResolutionStrategy() const;
    std::vector<EditOperation> ResolveConflicts();

    // State
    void Clear();

private:
    std::unordered_map<uint32_t, CollaboratorInfo> m_peers;
    std::vector<EditOperation> m_localOps;
    std::vector<EditOperation> m_remoteOps;
    std::vector<EditOperation> m_opLog;
    ResolutionStrategy m_strategy = ResolutionStrategy::LastWriterWins;
    uint64_t m_nextSeq = 1;
};

} // namespace atlas::graphvm
