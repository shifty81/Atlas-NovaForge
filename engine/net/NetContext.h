#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <queue>
#include <functional>

namespace atlas::ecs { class World; }
namespace atlas::net { class NetHardening; }

namespace atlas::net {

/// Version of the network packet schema.
/// Increment when the Packet struct layout changes.
constexpr uint32_t NET_PACKET_SCHEMA_VERSION = 1;

enum class NetMode {
    Standalone,
    Client,
    Server,
    P2P_Host,
    P2P_Peer
};

struct Packet {
    uint16_t type = 0;
    uint16_t size = 0;
    uint32_t tick = 0;
    uint32_t checksum = 0;
    std::vector<uint8_t> payload;
};

struct NetPeer {
    uint32_t id = 0;
    float rtt = 0.0f;
    bool connected = false;
};

struct InputFrame {
    uint32_t tick = 0;
    uint32_t playerID = 0;
    float moveX = 0.0f;
    float moveY = 0.0f;
};

struct WorldSnapshot {
    uint32_t tick = 0;
    std::vector<uint8_t> ecsState;
};

struct QueuedPacket {
    uint32_t destPeerID = 0; // 0 = broadcast
    Packet packet;
};

class NetContext {
public:
    void Init(NetMode mode);
    void Shutdown();

    void Poll();
    void Send(uint32_t peerID, const Packet& pkt);
    void Broadcast(const Packet& pkt);
    void Flush();

    NetMode Mode() const;
    const std::vector<NetPeer>& Peers() const;
    bool IsAuthority() const;

    // Peer management
    uint32_t AddPeer();
    void RemovePeer(uint32_t peerID);

    // Receive incoming packets (from local queue after Poll)
    bool Receive(Packet& outPkt);

    // ECS world binding (required for snapshot/rollback)
    void SetWorld(ecs::World* world);

    // Input frame recording for replay
    void RecordInput(const InputFrame& frame);
    const std::vector<InputFrame>& RecordedInputs() const;

    // Set callback for applying input frames during replay
    void SetInputApplyCallback(std::function<void(const InputFrame&)> cb);

    // Lockstep / Rollback
    void SaveSnapshot(uint32_t tick);
    void RollbackTo(uint32_t tick);
    void ReplayFrom(uint32_t tick);

    const std::vector<WorldSnapshot>& Snapshots() const;

    // Save tick broadcasting
    void BroadcastSaveTick(uint32_t tick, uint64_t stateHash);
    uint32_t LastSaveTick() const;
    uint64_t LastSaveHash() const;

    /// Returns the packet schema version this context was built with.
    static uint32_t PacketSchemaVersion() { return NET_PACKET_SCHEMA_VERSION; }

    /// Set a NetHardening instance for bandwidth/loss enforcement.
    void SetHardening(NetHardening* hardening);

    /// Number of packets dropped due to hardening (bandwidth/loss).
    uint32_t DroppedSendCount() const;

    /// Number of packets dropped due to invalid checksum on receive.
    uint32_t InvalidChecksumCount() const;

    /// Compute a CRC32 checksum over data.
    static uint32_t ComputeChecksum(const uint8_t* data, size_t size);

    /// Validate a packet's checksum field against its payload.
    static bool ValidateChecksum(const Packet& pkt);

private:
    NetMode m_mode = NetMode::Standalone;
    std::vector<NetPeer> m_peers;
    std::vector<WorldSnapshot> m_snapshots;
    std::vector<InputFrame> m_inputHistory;
    uint32_t m_nextPeerID = 1;

    // Bound ECS world for serialization
    ecs::World* m_world = nullptr;

    // Local packet queues for testability
    std::queue<QueuedPacket> m_outgoing;
    std::queue<Packet> m_incoming;

    // Optional callback for applying input frames during replay
    std::function<void(const InputFrame&)> m_inputApplyCallback;

    // Save tick broadcasting state
    uint32_t m_lastSaveTick = 0;
    uint64_t m_lastSaveHash = 0;

    // Hardening integration
    NetHardening* m_hardening = nullptr;
    uint32_t m_droppedSendCount = 0;
    uint32_t m_invalidChecksumCount = 0;
};

}
