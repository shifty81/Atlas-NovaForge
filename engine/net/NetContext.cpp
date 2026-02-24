#include "NetContext.h"
#include "NetHardening.h"
#include "../ecs/ECS.h"
#include <algorithm>
#include <cstring>

namespace atlas::net {

void NetContext::Init(NetMode mode) {
    m_mode = mode;
    m_peers.clear();
    m_snapshots.clear();
    m_inputHistory.clear();
    m_nextPeerID = 1;
    m_hardening = nullptr;
    m_droppedSendCount = 0;
    m_invalidChecksumCount = 0;
    while (!m_outgoing.empty()) m_outgoing.pop();
    while (!m_incoming.empty()) m_incoming.pop();
}

void NetContext::Shutdown() {
    m_peers.clear();
    m_snapshots.clear();
    m_inputHistory.clear();
    while (!m_outgoing.empty()) m_outgoing.pop();
    while (!m_incoming.empty()) m_incoming.pop();
    m_mode = NetMode::Standalone;
}

void NetContext::Poll() {
    // In local/loopback mode, move outgoing packets to incoming
    // This enables testing without real sockets
    while (!m_outgoing.empty()) {
        m_incoming.push(m_outgoing.front().packet);
        m_outgoing.pop();
    }
}

void NetContext::Send(uint32_t peerID, const Packet& pkt) {
    if (m_hardening) {
        if (!m_hardening->CanSendBytes(static_cast<uint32_t>(pkt.payload.size()))) {
            m_droppedSendCount++;
            return;
        }
        if (m_hardening->ShouldDropPacket()) {
            m_droppedSendCount++;
            return;
        }
    }

    QueuedPacket qp;
    qp.destPeerID = peerID;
    qp.packet = pkt;
    qp.packet.checksum = ComputeChecksum(pkt.payload.data(), pkt.payload.size());
    m_outgoing.push(qp);

    if (m_hardening) {
        m_hardening->RecordBytesSent(static_cast<uint32_t>(pkt.payload.size()));
        m_hardening->RecordPacketSent();
    }
}

void NetContext::Broadcast(const Packet& pkt) {
    if (m_hardening) {
        if (!m_hardening->CanSendBytes(static_cast<uint32_t>(pkt.payload.size()))) {
            m_droppedSendCount++;
            return;
        }
        if (m_hardening->ShouldDropPacket()) {
            m_droppedSendCount++;
            return;
        }
    }

    QueuedPacket qp;
    qp.destPeerID = 0; // 0 = broadcast
    qp.packet = pkt;
    qp.packet.checksum = ComputeChecksum(pkt.payload.data(), pkt.payload.size());
    m_outgoing.push(qp);

    if (m_hardening) {
        m_hardening->RecordBytesSent(static_cast<uint32_t>(pkt.payload.size()));
        m_hardening->RecordPacketSent();
    }
}

void NetContext::Flush() {
    // In a real implementation, this would push buffered data to sockets
    // For local mode, Poll() already handles the transfer
}

NetMode NetContext::Mode() const {
    return m_mode;
}

const std::vector<NetPeer>& NetContext::Peers() const {
    return m_peers;
}

bool NetContext::IsAuthority() const {
    return m_mode == NetMode::Server || m_mode == NetMode::P2P_Host;
}

uint32_t NetContext::AddPeer() {
    NetPeer peer;
    peer.id = m_nextPeerID++;
    peer.rtt = 0.0f;
    peer.connected = true;
    m_peers.push_back(peer);
    return peer.id;
}

void NetContext::RemovePeer(uint32_t peerID) {
    m_peers.erase(
        std::remove_if(m_peers.begin(), m_peers.end(),
            [peerID](const NetPeer& p) { return p.id == peerID; }),
        m_peers.end()
    );
}

bool NetContext::Receive(Packet& outPkt) {
    if (m_incoming.empty()) return false;
    outPkt = m_incoming.front();
    m_incoming.pop();
    if (!ValidateChecksum(outPkt)) {
        m_invalidChecksumCount++;
        return false;
    }
    return true;
}

void NetContext::SetWorld(ecs::World* world) {
    m_world = world;
}

void NetContext::RecordInput(const InputFrame& frame) {
    m_inputHistory.push_back(frame);
}

const std::vector<InputFrame>& NetContext::RecordedInputs() const {
    return m_inputHistory;
}

void NetContext::SetInputApplyCallback(std::function<void(const InputFrame&)> cb) {
    m_inputApplyCallback = std::move(cb);
}

void NetContext::SaveSnapshot(uint32_t tick) {
    WorldSnapshot snap;
    snap.tick = tick;
    if (m_world) {
        snap.ecsState = m_world->Serialize();
    }
    m_snapshots.push_back(std::move(snap));
}

void NetContext::RollbackTo(uint32_t tick) {
    // Find the snapshot for the requested tick
    const WorldSnapshot* target = nullptr;
    for (const auto& snap : m_snapshots) {
        if (snap.tick == tick) {
            target = &snap;
            break;
        }
    }

    if (target && m_world && !target->ecsState.empty()) {
        m_world->Deserialize(target->ecsState);
    }

    // Remove snapshots after the rollback tick
    m_snapshots.erase(
        std::remove_if(m_snapshots.begin(), m_snapshots.end(),
            [tick](const WorldSnapshot& s) { return s.tick > tick; }),
        m_snapshots.end()
    );
}

void NetContext::ReplayFrom(uint32_t tick) {
    // Replay recorded input frames from the given tick onward
    if (!m_world) return;

    for (const auto& frame : m_inputHistory) {
        if (frame.tick >= tick) {
            // Apply the input frame to the world before ticking
            if (m_inputApplyCallback) {
                m_inputApplyCallback(frame);
            }
            m_world->Update(1.0f / 60.0f);
        }
    }
}

const std::vector<WorldSnapshot>& NetContext::Snapshots() const {
    return m_snapshots;
}

void NetContext::BroadcastSaveTick(uint32_t tick, uint64_t stateHash) {
    m_lastSaveTick = tick;
    m_lastSaveHash = stateHash;

    Packet pkt;
    pkt.type = 0xFF00;
    pkt.tick = tick;

    // Pack tick (4 bytes) + hash (8 bytes) into payload
    pkt.payload.resize(sizeof(uint32_t) + sizeof(uint64_t));
    std::memcpy(pkt.payload.data(), &tick, sizeof(uint32_t));
    std::memcpy(pkt.payload.data() + sizeof(uint32_t), &stateHash, sizeof(uint64_t));
    pkt.size = static_cast<uint16_t>(pkt.payload.size());

    Broadcast(pkt);
}

uint32_t NetContext::LastSaveTick() const {
    return m_lastSaveTick;
}

uint64_t NetContext::LastSaveHash() const {
    return m_lastSaveHash;
}

void NetContext::SetHardening(NetHardening* hardening) {
    m_hardening = hardening;
}

uint32_t NetContext::DroppedSendCount() const {
    return m_droppedSendCount;
}

uint32_t NetContext::InvalidChecksumCount() const {
    return m_invalidChecksumCount;
}

uint32_t NetContext::ComputeChecksum(const uint8_t* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

bool NetContext::ValidateChecksum(const Packet& pkt) {
    if (pkt.checksum == 0 && pkt.payload.empty()) return true;
    uint32_t computed = ComputeChecksum(pkt.payload.data(), pkt.payload.size());
    return computed == pkt.checksum;
}

}
