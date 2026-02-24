#pragma once
// ============================================================
// Atlas QoS Packet Scheduler — Priority-Based Packet Ordering
// ============================================================
//
// Provides quality-of-service scheduling for outbound network
// packets. Each packet is assigned a priority and optional
// channel. The scheduler orders packets by priority (highest
// first) and enforces per-second bandwidth budgets with
// congestion detection.
//
// See: docs/05_NETWORKING.md
//      docs/ATLAS_CORE_CONTRACT.md

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace atlas::net {

/// Priority levels for network packets (higher = more important).
enum class PacketPriority : uint8_t {
    Low        = 0,
    Normal     = 1,
    High       = 2,
    Critical   = 3
};

/// Congestion state of the network link.
enum class CongestionState {
    Clear,        ///< No congestion detected
    Approaching,  ///< Usage nearing budget
    Congested     ///< Budget exceeded — low-priority packets deferred
};

/// A queued outbound packet.
struct QoSPacket {
    uint32_t id = 0;
    PacketPriority priority = PacketPriority::Normal;
    uint32_t sizeBytes = 0;
    uint8_t channel = 0;       ///< Logical channel (0 = default)
    uint64_t enqueueTick = 0;  ///< Tick when the packet was enqueued
};

/// Configuration for the QoS scheduler.
struct QoSConfig {
    /// Maximum bytes per second budget. 0 = unlimited.
    uint32_t bandwidthBudgetBytesPerSec = 0;

    /// Fraction of budget at which congestion warning fires (0.0–1.0).
    float congestionThreshold = 0.8f;

    /// Maximum number of queued packets. Oldest low-priority packets
    /// are dropped when the queue exceeds this limit.
    uint32_t maxQueueSize = 256;
};

/// Priority-based packet scheduler with congestion detection.
class QoSScheduler {
public:
    void Configure(const QoSConfig& config);
    const QoSConfig& Config() const;

    /// Enqueue a packet for scheduling.
    /// Returns false if the queue is full and the packet was dropped.
    bool Enqueue(const QoSPacket& packet);

    /// Dequeue the next packet to send according to priority ordering.
    /// Returns false if the queue is empty.
    bool Dequeue(QoSPacket& out);

    /// Number of packets currently queued.
    size_t QueueSize() const;

    /// True if the queue contains any packets.
    bool HasPending() const;

    /// Drain all packets from the queue in priority order.
    std::vector<QoSPacket> DrainAll();

    // --- Bandwidth & Congestion ---

    /// Record that bytes were actually sent this tick.
    void RecordBytesSent(uint32_t bytes);

    /// Call once per second (or per tick with deltaMs) to advance
    /// the bandwidth tracking window.
    void UpdateWindow(float deltaMs);

    /// Bytes sent in the current tracking window.
    uint32_t BytesSentThisWindow() const;

    /// Current congestion state.
    CongestionState Congestion() const;

    /// Total packets dropped due to queue overflow.
    uint32_t DroppedCount() const;

    /// Reset all statistics and clear the queue.
    void Reset();

private:
    void SortQueue();
    void EnforceSizeLimit();

    QoSConfig m_config;
    std::vector<QoSPacket> m_queue;
    uint32_t m_nextId = 1;
    uint32_t m_bytesSentThisWindow = 0;
    float m_windowAccumulator = 0.0f;
    uint32_t m_droppedCount = 0;
};

}  // namespace atlas::net
