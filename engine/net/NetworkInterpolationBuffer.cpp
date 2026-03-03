#include "NetworkInterpolationBuffer.h"
#include <algorithm>

namespace atlas::net {

NetworkInterpolationBuffer::NetworkInterpolationBuffer(size_t bufferDepth,
                                                       float maxExtrapolationTicks)
    : m_bufferDepth(bufferDepth)
    , m_maxExtrapolationTicks(maxExtrapolationTicks)
{}

// ── Snapshot recording ─────────────────────────────────────────────

void NetworkInterpolationBuffer::PushSnapshot(const EntitySnapshot& snap) {
    EntityBuffer* buf = findEntity(snap.entityId);
    if (!buf) {
        m_entities.push_back({snap.entityId, {}});
        buf = &m_entities.back();
    }

    // Insert in tick order (usually appended at the end).
    auto& snaps = buf->snapshots;
    auto it = std::lower_bound(
        snaps.begin(), snaps.end(), snap,
        [](const EntitySnapshot& a, const EntitySnapshot& b) {
            return a.tick < b.tick;
        });
    // Replace if same tick already present.
    if (it != snaps.end() && it->tick == snap.tick) {
        *it = snap;
    } else {
        snaps.insert(it, snap);
    }

    // Trim to buffer depth (keep most recent).
    while (snaps.size() > m_bufferDepth) {
        snaps.erase(snaps.begin());
    }
}

// ── Interpolation ──────────────────────────────────────────────────

InterpolatedState NetworkInterpolationBuffer::Interpolate(
    uint32_t entityId, float renderTick) const {
    const EntityBuffer* buf = findEntity(entityId);
    if (!buf || buf->snapshots.empty()) {
        return InterpolatedState{entityId, 0, 0, 0, 0, false};
    }

    const auto& snaps = buf->snapshots;

    // If renderTick is at or before the first snapshot, clamp to first.
    if (renderTick <= static_cast<float>(snaps.front().tick)) {
        const auto& s = snaps.front();
        return InterpolatedState{entityId, s.posX, s.posY, s.posZ, s.rotYaw, true};
    }

    // If renderTick is beyond the latest snapshot, extrapolate.
    if (renderTick > static_cast<float>(snaps.back().tick)) {
        float ticksBeyond = renderTick - static_cast<float>(snaps.back().tick);
        if (ticksBeyond > m_maxExtrapolationTicks) {
            return InterpolatedState{entityId, 0, 0, 0, 0, false};
        }
        return extrapolate(snaps.back(), ticksBeyond);
    }

    // Find the two bounding snapshots and LERP.
    for (size_t i = 0; i + 1 < snaps.size(); ++i) {
        float tickA = static_cast<float>(snaps[i].tick);
        float tickB = static_cast<float>(snaps[i + 1].tick);
        if (renderTick >= tickA && renderTick <= tickB) {
            float range = tickB - tickA;
            float t = (range > 0.0f) ? ((renderTick - tickA) / range) : 0.0f;
            return lerp(snaps[i], snaps[i + 1], t);
        }
    }

    // Fallback: use latest snapshot.
    const auto& s = snaps.back();
    return InterpolatedState{entityId, s.posX, s.posY, s.posZ, s.rotYaw, true};
}

// ── Removal ────────────────────────────────────────────────────────

void NetworkInterpolationBuffer::RemoveEntity(uint32_t entityId) {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [entityId](const EntityBuffer& eb) {
                return eb.entityId == entityId;
            }),
        m_entities.end());
}

void NetworkInterpolationBuffer::Clear() {
    m_entities.clear();
}

// ── Queries ────────────────────────────────────────────────────────

size_t NetworkInterpolationBuffer::SnapshotCount(uint32_t entityId) const {
    const EntityBuffer* buf = findEntity(entityId);
    return buf ? buf->snapshots.size() : 0;
}

// ── Internals ──────────────────────────────────────────────────────

NetworkInterpolationBuffer::EntityBuffer*
NetworkInterpolationBuffer::findEntity(uint32_t id) {
    for (auto& eb : m_entities) {
        if (eb.entityId == id) return &eb;
    }
    return nullptr;
}

const NetworkInterpolationBuffer::EntityBuffer*
NetworkInterpolationBuffer::findEntity(uint32_t id) const {
    for (const auto& eb : m_entities) {
        if (eb.entityId == id) return &eb;
    }
    return nullptr;
}

InterpolatedState NetworkInterpolationBuffer::lerp(
    const EntitySnapshot& a, const EntitySnapshot& b, float t) {
    InterpolatedState result;
    result.entityId = a.entityId;
    result.posX     = a.posX + (b.posX - a.posX) * t;
    result.posY     = a.posY + (b.posY - a.posY) * t;
    result.posZ     = a.posZ + (b.posZ - a.posZ) * t;
    result.rotYaw   = a.rotYaw + (b.rotYaw - a.rotYaw) * t;
    result.valid    = true;
    return result;
}

InterpolatedState NetworkInterpolationBuffer::extrapolate(
    const EntitySnapshot& snap, float ticksBeyond) {
    InterpolatedState result;
    result.entityId = snap.entityId;
    result.posX     = snap.posX + snap.velX * ticksBeyond;
    result.posY     = snap.posY + snap.velY * ticksBeyond;
    result.posZ     = snap.posZ + snap.velZ * ticksBeyond;
    result.rotYaw   = snap.rotYaw;
    result.valid    = true;
    return result;
}

} // namespace atlas::net
