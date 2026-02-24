#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace atlas::net {

/// Configuration for production networking hardening features.
struct NetHardeningConfig {
    /// Connection timeout in milliseconds. 0 = disabled.
    uint32_t connectionTimeoutMs = 10000;

    /// Maximum reconnection attempts. 0 = no reconnection.
    uint32_t maxReconnectAttempts = 5;

    /// Delay between reconnection attempts in milliseconds.
    uint32_t reconnectDelayMs = 2000;

    /// Maximum outbound bandwidth in bytes/sec. 0 = unlimited.
    uint32_t maxBandwidthBytesPerSec = 0;

    /// Maximum packet size in bytes.
    uint32_t maxPacketSize = 1400;

    /// Heartbeat interval in milliseconds.
    uint32_t heartbeatIntervalMs = 1000;

    /// Number of missed heartbeats before disconnect.
    uint32_t heartbeatMissThreshold = 5;
};

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
    TimedOut,
    Kicked
};

enum class ConnectionQuality {
    Excellent,   // RTT < 30ms, loss < 1%
    Good,        // RTT < 80ms, loss < 3%
    Fair,        // RTT < 150ms, loss < 8%
    Poor,        // RTT < 300ms, loss < 15%
    Critical     // everything else
};

struct PacketLossSimConfig {
    float lossPercent = 0.0f;       // 0.0 to 100.0
    float latencyMs = 0.0f;         // additional latency
    float jitterMs = 0.0f;          // jitter range
    bool enabled = false;
};

struct ConnectionStats {
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint32_t packetsSent = 0;
    uint32_t packetsReceived = 0;
    uint32_t packetsDropped = 0;
    float averageRttMs = 0.0f;
    float peakRttMs = 0.0f;
    uint32_t reconnectCount = 0;
    ConnectionState state = ConnectionState::Disconnected;
};

/// Production hardening layer on top of NetContext.
/// Provides connection timeouts, reconnection, bandwidth throttling,
/// and heartbeat-based connection monitoring.
class NetHardening {
public:
    using StateCallback = std::function<void(ConnectionState oldState, ConnectionState newState)>;

    void Configure(const NetHardeningConfig& config);
    const NetHardeningConfig& Config() const;

    /// Called every frame/tick to update timers and state.
    void Update(float deltaTimeMs);

    /// Begin a connection attempt.
    void Connect();

    /// Disconnect cleanly.
    void Disconnect();

    /// Record that bytes were sent (for bandwidth tracking).
    void RecordBytesSent(uint32_t bytes);

    /// Record that bytes were received.
    void RecordBytesReceived(uint32_t bytes);

    /// Record a packet sent.
    void RecordPacketSent();

    /// Record a packet received.
    void RecordPacketReceived();

    /// Record a dropped packet.
    void RecordPacketDropped();

    /// Record an RTT measurement.
    void RecordRtt(float rttMs);

    /// Record a heartbeat received from the remote side.
    void RecordHeartbeat();

    /// Check if a packet can be sent given bandwidth limits.
    /// Returns true if within budget for this frame.
    bool CanSendBytes(uint32_t bytes) const;

    /// Check if a packet size is within limits.
    bool IsPacketSizeValid(uint32_t bytes) const;

    /// Get current connection state.
    ConnectionState State() const;

    /// Get connection statistics.
    const ConnectionStats& Stats() const;

    /// Set a callback for state changes.
    void SetStateCallback(StateCallback cb);

    /// Reset statistics.
    void ResetStats();

    void SetPacketLossSimulation(const PacketLossSimConfig& config);
    const PacketLossSimConfig& PacketLossSimulation() const;
    bool ShouldDropPacket() const;

    ConnectionQuality GetConnectionQuality() const;
    std::string ConnectionQualityString() const;

    /// Returns simulated latency = latencyMs + deterministic jitter in [-jitterMs, +jitterMs].
    uint32_t GetSimulatedLatencyMs() const;

    float PacketLossPercent() const;
    float AverageBandwidthBytesPerSec() const;

private:
    void SetState(ConnectionState newState);
    void HandleTimeout();
    void AttemptReconnect();

    NetHardeningConfig m_config;
    ConnectionStats m_stats;
    StateCallback m_stateCallback;

    float m_timeSinceLastHeartbeat = 0.0f;
    float m_timeSinceConnectStart = 0.0f;
    float m_timeSinceLastReconnect = 0.0f;
    uint32_t m_reconnectAttempts = 0;
    float m_bytesSentThisSecond = 0.0f;
    float m_secondAccumulator = 0.0f;

    PacketLossSimConfig m_lossSimConfig;
    mutable uint32_t m_lossCounter = 0;
    float m_totalBytesTracked = 0.0f;
    float m_totalTimeTracked = 0.0f;
};

}
