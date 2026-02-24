#include "TileChunkBuilder.h"
#include <map>
#include <cmath>

namespace atlas::tile {

void TileChunkBuilder::Build(
    const atlas::editor::TileMap& map,
    const atlas::editor::TileLayer& layer,
    const ChunkCoord& chunkOrigin,
    TileChunk& outChunk)
{
    outChunk.Clear();
    outChunk.originX  = chunkOrigin.cx;
    outChunk.originY  = chunkOrigin.cy;
    outChunk.chunkSize = kChunkSize;
    outChunk.dirty    = false;

    const float cellSize = static_cast<float>(map.gridCellSize);

    // Collect tiles inside the chunk using a sorted container so the
    // output is deterministic regardless of unordered_map iteration order.
    std::map<std::pair<int32_t,int32_t>, atlas::editor::TileInstance> sorted;
    for (auto& [coord, tile] : layer.tiles) {
        if (IsInsideChunk(coord, chunkOrigin)) {
            sorted[{coord.x, coord.y}] = tile;
        }
    }

    for (auto& [key, tile] : sorted) {
        float wx = key.first  * cellSize;
        float wy = key.second * cellSize;

        // UV defaults — a real atlas lookup would replace these.
        float u0 = 0.0f, v0 = 0.0f;
        float u1 = 1.0f, v1 = 1.0f;

        // Handle flip flags
        if (tile.flippedX) { std::swap(u0, u1); }
        if (tile.flippedY) { std::swap(v0, v1); }

        uint32_t base = static_cast<uint32_t>(outChunk.vertices.size());

        // Emit 4 vertices per tile
        outChunk.vertices.push_back({wx,            wy,            u0, v0});
        outChunk.vertices.push_back({wx + cellSize, wy,            u1, v0});
        outChunk.vertices.push_back({wx + cellSize, wy + cellSize, u1, v1});
        outChunk.vertices.push_back({wx,            wy + cellSize, u0, v1});

        // Emit 6 indices per tile (two triangles)
        outChunk.indices.push_back(base + 0);
        outChunk.indices.push_back(base + 1);
        outChunk.indices.push_back(base + 2);
        outChunk.indices.push_back(base + 0);
        outChunk.indices.push_back(base + 2);
        outChunk.indices.push_back(base + 3);
    }
}

ChunkCoord TileChunkBuilder::WorldToChunk(const atlas::editor::GridCoord& coord) {
    // Use floored integer division so negative coords map correctly.
    auto floorDiv = [](int32_t a, int32_t b) -> int32_t {
        return (a >= 0) ? (a / b) : ((a - b + 1) / b);
    };
    return {floorDiv(coord.x, kChunkSize), floorDiv(coord.y, kChunkSize)};
}

bool TileChunkBuilder::IsInsideChunk(const atlas::editor::GridCoord& coord,
                                     const ChunkCoord& chunkOrigin) {
    int32_t lx = coord.x - chunkOrigin.cx * kChunkSize;
    int32_t ly = coord.y - chunkOrigin.cy * kChunkSize;
    return lx >= 0 && lx < kChunkSize && ly >= 0 && ly < kChunkSize;
}

void TileChunkBuilder::MarkDirty(std::set<ChunkCoord>& dirtySet,
                                 const atlas::editor::GridCoord& coord) {
    dirtySet.insert(WorldToChunk(coord));
}

} // namespace atlas::tile
