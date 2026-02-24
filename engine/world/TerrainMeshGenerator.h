#pragma once
#include "WorldLayout.h"
#include <vector>
#include <functional>

namespace atlas::world {

struct Vertex {
    float x, y, z;       // position
    float nx, ny, nz;    // normal
};

struct TerrainMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

// Height function: given (x, z) world coords, returns height
using HeightFunc = std::function<float(float, float)>;

class TerrainMeshGenerator {
public:
    // Generate a flat grid mesh for a chunk with resolution x resolution quads
    static TerrainMesh Generate(
        const ChunkCoord& chunk,
        int resolution,
        float chunkSize,
        const HeightFunc& heightFn = nullptr
    );

    // Compute a face normal for a triangle
    static void ComputeNormal(
        float ax, float ay, float az,
        float bx, float by, float bz,
        float cx, float cy, float cz,
        float& nx, float& ny, float& nz
    );
};

}
