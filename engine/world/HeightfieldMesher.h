#pragma once
#include <vector>
#include <cstdint>

namespace atlas::world {

struct Heightfield {
    int size = 0;
    float scale = 1.0f;
    std::vector<float> data;

    float At(int x, int z) const {
        if (x < 0 || x >= size || z < 0 || z >= size) return 0.0f;
        return data[static_cast<size_t>(z) * size + x];
    }
};

struct MeshData {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<uint32_t> indices;
};

class HeightfieldMesher {
public:
    // Build a grid mesh from a heightfield with LOD support (step = 1 << lod)
    static MeshData BuildMesh(const Heightfield& hf, int lod = 0);

    // Compute per-vertex normals from triangle data
    static void ComputeNormals(MeshData& mesh);
};

}
