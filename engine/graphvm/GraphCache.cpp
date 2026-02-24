#include "graphvm/GraphCache.h"

namespace atlas::graphvm {

bool GraphCache::Has(uint64_t key) const {
    return m_cache.find(key) != m_cache.end();
}

const CacheEntry* GraphCache::Get(uint64_t key) const {
    auto it = m_cache.find(key);
    if (it == m_cache.end()) return nullptr;
    return &it->second;
}

void GraphCache::Store(uint64_t key, const CacheEntry& entry) {
    m_cache[key] = entry;
}

void GraphCache::Invalidate(uint64_t key) {
    m_cache.erase(key);
}

void GraphCache::InvalidateAll() {
    m_cache.clear();
}

size_t GraphCache::Size() const {
    return m_cache.size();
}

void GraphCache::EvictBefore(uint32_t tick) {
    for (auto it = m_cache.begin(); it != m_cache.end(); ) {
        if (it->second.tick < tick) {
            it = m_cache.erase(it);
        } else {
            ++it;
        }
    }
}

uint64_t GraphCache::HashKey(uint32_t graphID, uint64_t seed, int32_t lod) {
    uint64_t h = static_cast<uint64_t>(graphID);
    h = (h ^ (seed * 0x9E3779B97F4A7C15ULL)) * 0xBF58476D1CE4E5B9ULL;
    h = (h ^ (static_cast<uint64_t>(static_cast<uint32_t>(lod)) * 0x94D049BB133111EBULL)) * 0x9E3779B97F4A7C15ULL;
    return h;
}

}
