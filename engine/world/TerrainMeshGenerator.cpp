#include "TerrainMeshGenerator.h"
#include <cmath>

namespace atlas::world {

void TerrainMeshGenerator::ComputeNormal(
    float ax, float ay, float az,
    float bx, float by, float bz,
    float cx, float cy, float cz,
    float& nx, float& ny, float& nz)
{
    float ux = bx - ax, uy = by - ay, uz = bz - az;
    float vx = cx - ax, vy = cy - ay, vz = cz - az;

    nx = uy * vz - uz * vy;
    ny = uz * vx - ux * vz;
    nz = ux * vy - uy * vx;

    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0f) {
        nx /= len;
        ny /= len;
        nz /= len;
    }
}

TerrainMesh TerrainMeshGenerator::Generate(
    const ChunkCoord& chunk,
    int resolution,
    float chunkSize,
    const HeightFunc& heightFn)
{
    TerrainMesh mesh;

    if (resolution < 1) resolution = 1;

    int vertsPerSide = resolution + 1;
    float step = chunkSize / static_cast<float>(resolution);

    float originX = chunk.x * chunkSize;
    float originZ = chunk.z * chunkSize;

    // Generate vertices
    mesh.vertices.resize(static_cast<size_t>(vertsPerSide) * vertsPerSide);

    for (int iy = 0; iy <= resolution; ++iy) {
        for (int ix = 0; ix <= resolution; ++ix) {
            float wx = originX + ix * step;
            float wz = originZ + iy * step;
            float wy = heightFn ? heightFn(wx, wz) : 0.0f;

            size_t idx = static_cast<size_t>(iy) * vertsPerSide + ix;
            mesh.vertices[idx] = {wx, wy, wz, 0.0f, 1.0f, 0.0f};
        }
    }

    // Generate indices (two triangles per quad)
    mesh.indices.reserve(static_cast<size_t>(resolution) * resolution * 6);

    for (int iy = 0; iy < resolution; ++iy) {
        for (int ix = 0; ix < resolution; ++ix) {
            uint32_t topLeft     = static_cast<uint32_t>(iy * vertsPerSide + ix);
            uint32_t topRight    = topLeft + 1;
            uint32_t bottomLeft  = static_cast<uint32_t>((iy + 1) * vertsPerSide + ix);
            uint32_t bottomRight = bottomLeft + 1;

            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);

            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }

    // Compute per-vertex normals by averaging face normals
    if (heightFn) {
        // Reset normals
        for (auto& v : mesh.vertices) {
            v.nx = 0; v.ny = 0; v.nz = 0;
        }

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            auto& a = mesh.vertices[mesh.indices[i]];
            auto& b = mesh.vertices[mesh.indices[i + 1]];
            auto& c = mesh.vertices[mesh.indices[i + 2]];

            float fnx, fny, fnz;
            ComputeNormal(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z,
                          fnx, fny, fnz);

            a.nx += fnx; a.ny += fny; a.nz += fnz;
            b.nx += fnx; b.ny += fny; b.nz += fnz;
            c.nx += fnx; c.ny += fny; c.nz += fnz;
        }

        for (auto& v : mesh.vertices) {
            float len = std::sqrt(v.nx * v.nx + v.ny * v.ny + v.nz * v.nz);
            if (len > 0.0f) {
                v.nx /= len;
                v.ny /= len;
                v.nz /= len;
            }
        }
    }

    return mesh;
}

}
