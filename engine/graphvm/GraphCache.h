#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>

namespace atlas::graphvm {

struct CacheEntry {
    uint64_t hash;
    std::vector<float> data;
    uint32_t tick;
};

class GraphCache {
public:
    bool Has(uint64_t key) const;
    const CacheEntry* Get(uint64_t key) const;
    void Store(uint64_t key, const CacheEntry& entry);
    void Invalidate(uint64_t key);
    void InvalidateAll();
    size_t Size() const;

    // Evict entries older than given tick
    void EvictBefore(uint32_t tick);

    // Compute a simple hash for cache keys
    static uint64_t HashKey(uint32_t graphID, uint64_t seed, int32_t lod);
private:
    std::unordered_map<uint64_t, CacheEntry> m_cache;
};

}
