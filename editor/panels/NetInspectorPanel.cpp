#include "NetInspectorPanel.h"
#include <cstdio>

namespace atlas::editor {

void NetInspectorPanel::Draw() {
    auto mode = m_net.Mode();
    switch (mode) {
        case net::NetMode::Standalone: m_snapshot.modeString = "Standalone"; break;
        case net::NetMode::Client:     m_snapshot.modeString = "Client"; break;
        case net::NetMode::Server:     m_snapshot.modeString = "Server"; break;
        case net::NetMode::P2P_Host:   m_snapshot.modeString = "P2P_Host"; break;
        case net::NetMode::P2P_Peer:   m_snapshot.modeString = "P2P_Peer"; break;
    }

    auto& peers = m_net.Peers();
    m_snapshot.peerCount = static_cast<uint32_t>(peers.size());
    m_snapshot.connectedCount = 0;
    float totalRtt = 0.0f;
    for (const auto& p : peers) {
        if (p.connected) {
            ++m_snapshot.connectedCount;
            totalRtt += p.rtt;
        }
    }
    m_snapshot.avgRtt = (m_snapshot.connectedCount > 0)
        ? totalRtt / static_cast<float>(m_snapshot.connectedCount)
        : 0.0f;

    m_drawList.Clear();

    // Background
    m_drawList.DrawRect({0, 0, 600, 400}, {30, 30, 30, 255});

    // Title bar
    m_drawList.DrawRect({0, 0, 600, 24}, {50, 50, 50, 255});
    m_drawList.DrawText({4, 4, 300, 20}, "Network Inspector", {220, 220, 220, 255});

    int32_t y = 28;

    // Mode
    m_drawList.DrawText({4, y, 590, 16}, "Mode: " + m_snapshot.modeString, {200, 200, 200, 255});
    y += 20;

    // Peer count
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Peers: %u  Connected: %u",
                  m_snapshot.peerCount, m_snapshot.connectedCount);
    m_drawList.DrawText({4, y, 590, 16}, buf, {200, 200, 200, 255});
    y += 20;

    // Average RTT
    std::snprintf(buf, sizeof(buf), "Avg RTT: %.1f ms", m_snapshot.avgRtt);
    m_drawList.DrawText({4, y, 590, 16}, buf, {200, 200, 200, 255});
    y += 20;

    // Peer list
    if (m_snapshot.peerCount > 0) {
        m_drawList.DrawRect({0, y, 600, 20}, {45, 45, 45, 255});
        m_drawList.DrawText({4, y + 2, 590, 16}, "Peer List", {180, 200, 220, 255});
        y += 22;

        for (const auto& p : peers) {
            atlas::ui::UIColor statusColor = p.connected
                ? atlas::ui::UIColor{100, 200, 100, 255}
                : atlas::ui::UIColor{200, 100, 100, 255};
            std::string status = p.connected ? "Connected" : "Disconnected";
            std::snprintf(buf, sizeof(buf), "Peer %u: %s  RTT: %.1f ms",
                          p.id, status.c_str(), p.rtt);
            m_drawList.DrawText({8, y, 580, 16}, buf, statusColor);
            y += 20;
        }
    } else {
        m_drawList.DrawText({4, y, 590, 16}, "No peers", {160, 160, 160, 255});
    }
}

}
