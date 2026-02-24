#pragma once
#include "WorldLayout.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdint>

namespace atlas::world {

enum class ChunkState {
    Unloaded,
    Loading,
    Loaded,
    Cached     // on disk but not in memory
};

struct ChunkEntry {
    ChunkCoord coord;
    ChunkState state = ChunkState::Unloaded;
    std::vector<uint8_t> data;   // raw chunk data
};

class WorldStreamer {
public:
    explicit WorldStreamer(const WorldLayout& layout, const std::string& cacheDir = "");

    // Update streaming based on viewer position
    void Update(const WorldPos& viewerPos, int lod, float loadRadius, float unloadRadius);

    // Request a specific chunk to be loaded
    bool RequestLoad(const ChunkCoord& chunk);

    // Mark a chunk as loaded with data
    void SetChunkData(const ChunkCoord& chunk, const std::vector<uint8_t>& data);

    // Unload a chunk (optionally cache to disk)
    void UnloadChunk(const ChunkCoord& chunk);

    // Query chunk state
    ChunkState GetChunkState(const ChunkCoord& chunk) const;

    // Get all loaded chunks
    std::vector<ChunkCoord> GetLoadedChunks() const;

    // Disk cache operations
    bool SaveChunkToCache(const ChunkCoord& chunk) const;
    bool LoadChunkFromCache(const ChunkCoord& chunk);

    // Stats
    size_t LoadedCount() const;
    size_t CachedCount() const;

private:
    const WorldLayout& m_layout;
    std::string m_cacheDir;
    std::unordered_map<uint64_t, ChunkEntry> m_chunks;

    uint64_t MakeKey(const ChunkCoord& chunk) const;
    std::string CacheFilePath(const ChunkCoord& chunk) const;
};

}
