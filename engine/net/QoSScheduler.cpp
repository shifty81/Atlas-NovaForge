#include "QoSScheduler.h"
#include <algorithm>

namespace atlas::net {

void QoSScheduler::Configure(const QoSConfig& config) {
    m_config = config;
}

const QoSConfig& QoSScheduler::Config() const {
    return m_config;
}

bool QoSScheduler::Enqueue(const QoSPacket& packet) {
    if (m_config.maxQueueSize > 0 && m_queue.size() >= m_config.maxQueueSize) {
        // Try to drop a lower-priority packet to make room
        auto it = std::min_element(m_queue.begin(), m_queue.end(),
            [](const QoSPacket& a, const QoSPacket& b) {
                return static_cast<uint8_t>(a.priority) < static_cast<uint8_t>(b.priority);
            });

        if (it != m_queue.end() &&
            static_cast<uint8_t>(it->priority) < static_cast<uint8_t>(packet.priority)) {
            m_queue.erase(it);
            m_droppedCount++;
        } else {
            m_droppedCount++;
            return false;
        }
    }

    QoSPacket p = packet;
    p.id = m_nextId++;
    m_queue.push_back(p);
    return true;
}

bool QoSScheduler::Dequeue(QoSPacket& out) {
    if (m_queue.empty()) return false;

    SortQueue();
    out = m_queue.front();
    m_queue.erase(m_queue.begin());
    return true;
}

size_t QoSScheduler::QueueSize() const {
    return m_queue.size();
}

bool QoSScheduler::HasPending() const {
    return !m_queue.empty();
}

std::vector<QoSPacket> QoSScheduler::DrainAll() {
    SortQueue();
    std::vector<QoSPacket> result;
    result.swap(m_queue);
    return result;
}

void QoSScheduler::RecordBytesSent(uint32_t bytes) {
    m_bytesSentThisWindow += bytes;
}

void QoSScheduler::UpdateWindow(float deltaMs) {
    m_windowAccumulator += deltaMs;
    if (m_windowAccumulator >= 1000.0f) {
        m_windowAccumulator -= 1000.0f;
        m_bytesSentThisWindow = 0;
    }
}

uint32_t QoSScheduler::BytesSentThisWindow() const {
    return m_bytesSentThisWindow;
}

CongestionState QoSScheduler::Congestion() const {
    if (m_config.bandwidthBudgetBytesPerSec == 0)
        return CongestionState::Clear;

    float ratio = static_cast<float>(m_bytesSentThisWindow)
                  / static_cast<float>(m_config.bandwidthBudgetBytesPerSec);

    if (ratio >= 1.0f)
        return CongestionState::Congested;
    if (ratio >= m_config.congestionThreshold)
        return CongestionState::Approaching;
    return CongestionState::Clear;
}

uint32_t QoSScheduler::DroppedCount() const {
    return m_droppedCount;
}

void QoSScheduler::Reset() {
    m_queue.clear();
    m_bytesSentThisWindow = 0;
    m_windowAccumulator = 0.0f;
    m_droppedCount = 0;
    m_nextId = 1;
}

void QoSScheduler::SortQueue() {
    // Stable sort: highest priority first, then FIFO within same priority
    std::stable_sort(m_queue.begin(), m_queue.end(),
        [](const QoSPacket& a, const QoSPacket& b) {
            return static_cast<uint8_t>(a.priority) > static_cast<uint8_t>(b.priority);
        });
}

void QoSScheduler::EnforceSizeLimit() {
    while (m_config.maxQueueSize > 0 && m_queue.size() > m_config.maxQueueSize) {
        // Drop lowest-priority packet
        auto it = std::min_element(m_queue.begin(), m_queue.end(),
            [](const QoSPacket& a, const QoSPacket& b) {
                return static_cast<uint8_t>(a.priority) < static_cast<uint8_t>(b.priority);
            });
        if (it != m_queue.end()) {
            m_queue.erase(it);
            m_droppedCount++;
        }
    }
}

}  // namespace atlas::net
