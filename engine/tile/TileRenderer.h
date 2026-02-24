#pragma once
#include <cstdint>
#include <vector>

namespace atlas::tile {

/// Vertex emitted by the chunk builder for tile rendering.
struct TileVertex {
    float x = 0.0f;
    float y = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
};

/// A prebuilt chunk of tile mesh data ready for GPU upload.
struct TileChunk {
    int32_t  originX = 0;
    int32_t  originY = 0;
    int32_t  chunkSize = 8;         ///< Grid cells per side
    uint32_t atlasTextureId = 0;
    bool     dirty = true;

    std::vector<TileVertex>  vertices;
    std::vector<uint32_t>    indices;

    void Clear() { vertices.clear(); indices.clear(); dirty = true; }
    size_t VertexCount() const { return vertices.size(); }
    size_t IndexCount()  const { return indices.size(); }
};

/// Rendering parameters passed to the tile renderer per-frame.
struct TileRenderParams {
    uint32_t atlasTextureId = 0;
    bool     showGrid       = false;
    bool     showChunkBounds = false;
    float    cameraX = 0.0f;
    float    cameraY = 0.0f;
    float    zoom    = 1.0f;
    int32_t  viewportW = 1280;
    int32_t  viewportH = 720;
};

/// Backend-agnostic interface for rendering tile chunks.
/// Concrete implementations exist for OpenGL, Vulkan, DX11, etc.
class ITileRenderer {
public:
    virtual ~ITileRenderer() = default;

    virtual void Initialize() = 0;
    virtual void Shutdown()   = 0;

    virtual void BeginFrame(const TileRenderParams& params) = 0;
    virtual void EndFrame() = 0;

    virtual void SubmitChunk(const TileChunk& chunk) = 0;

    /// Optional: draw a grid overlay.
    virtual void DrawGrid(int32_t cellSize, int32_t viewW, int32_t viewH) = 0;
};

/// Null implementation for headless / server mode.
class NullTileRenderer : public ITileRenderer {
public:
    void Initialize() override {}
    void Shutdown()   override {}
    void BeginFrame(const TileRenderParams& /*params*/) override {}
    void EndFrame()   override {}
    void SubmitChunk(const TileChunk& /*chunk*/) override {}
    void DrawGrid(int32_t /*cellSize*/, int32_t /*viewW*/, int32_t /*viewH*/) override {}
};

} // namespace atlas::tile
