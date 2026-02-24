#include "HumanMeshGenerator.h"
#include <cmath>
#include <algorithm>

namespace atlas::character {

HumanMeshParams HumanMeshGenerator::ParamsFromGraph(
    const CharacterGraph& graph,
    CharacterNodeID bodyNode,
    CharacterNodeID faceNode,
    CharacterNodeID hairNode,
    CharacterNodeID materialNode)
{
    HumanMeshParams params;

    // Extract body proportions
    if (auto* body = graph.GetOutput(bodyNode, 0)) {
        if (body->data.size() >= 4) {
            params.height   = body->data[0];
            params.mass     = body->data[1];
            params.armRatio = body->data[2];
            params.legRatio = body->data[3];
        }
    }

    // Extract facial features
    if (auto* face = graph.GetOutput(faceNode, 0)) {
        if (face->data.size() >= 5) {
            params.noseWidth   = face->data[0];
            params.eyeSpacing  = face->data[1];
            params.jawWidth    = face->data[2];
            params.browHeight  = face->data[3];
            params.lipFullness = face->data[4];
        }
    }

    // Extract hair
    if (auto* hair = graph.GetOutput(hairNode, 0)) {
        if (hair->data.size() >= 6) {
            params.hairStyle   = hair->data[0];
            params.hairLength  = hair->data[1];
            params.hairDensity = hair->data[2];
            params.hairR       = hair->data[3];
            params.hairG       = hair->data[4];
            params.hairB       = hair->data[5];
        }
    }

    // Extract material
    if (auto* mat = graph.GetOutput(materialNode, 0)) {
        if (mat->data.size() >= 3) {
            params.skinTone  = mat->data[0];
            params.hairColor = mat->data[1];
            params.eyeColor  = mat->data[2];
        }
    }

    return params;
}

// ---------------------------------------------------------------------------
// Internal helpers – procedural human mesh geometry
// ---------------------------------------------------------------------------

static void AddQuad(procedural::MeshData& mesh,
                    float x0, float y0, float z0,
                    float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float nx, float ny, float nz)
{
    uint32_t base = static_cast<uint32_t>(mesh.vertices.size() / 3);
    mesh.vertices.insert(mesh.vertices.end(), {x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3});
    mesh.normals.insert(mesh.normals.end(), {nx, ny, nz, nx, ny, nz, nx, ny, nz, nx, ny, nz});
    mesh.indices.insert(mesh.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
}

static void AddBox(procedural::MeshData& mesh,
                   float cx, float cy, float cz,
                   float hw, float hh, float hd)
{
    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;
    float z0 = cz - hd, z1 = cz + hd;

    // Front
    AddQuad(mesh, x0,y0,z1, x1,y0,z1, x1,y1,z1, x0,y1,z1, 0,0,1);
    // Back
    AddQuad(mesh, x1,y0,z0, x0,y0,z0, x0,y1,z0, x1,y1,z0, 0,0,-1);
    // Right
    AddQuad(mesh, x1,y0,z1, x1,y0,z0, x1,y1,z0, x1,y1,z1, 1,0,0);
    // Left
    AddQuad(mesh, x0,y0,z0, x0,y0,z1, x0,y1,z1, x0,y1,z0, -1,0,0);
    // Top
    AddQuad(mesh, x0,y1,z1, x1,y1,z1, x1,y1,z0, x0,y1,z0, 0,1,0);
    // Bottom
    AddQuad(mesh, x0,y0,z0, x1,y0,z0, x1,y0,z1, x0,y0,z1, 0,-1,0);
}

procedural::MeshData HumanMeshGenerator::Generate(const HumanMeshParams& params) {
    procedural::MeshData mesh;

    // Scale factors derived from params
    float s = params.height / 1.8f;   // overall scale
    float massScale = 0.8f + (params.mass / 120.0f) * 0.4f;

    // --- Torso ---
    float torsoW = 0.20f * s * massScale;
    float torsoH = 0.30f * s;
    float torsoD = 0.12f * s * massScale;
    float torsoY = params.height * 0.55f;
    AddBox(mesh, 0, torsoY, 0, torsoW, torsoH, torsoD);

    // --- Head ---
    float headSize = 0.10f * s * params.jawWidth;
    float headY = torsoY + torsoH + headSize + 0.04f * s;
    AddBox(mesh, 0, headY, 0, headSize, headSize * 1.1f, headSize * 0.9f);

    // --- Upper arms ---
    float armW = 0.05f * s;
    float upperArmH = 0.15f * s * params.armRatio;
    float armY = torsoY + torsoH * 0.7f;
    float armX = torsoW + armW + 0.01f * s;
    AddBox(mesh,  armX, armY - upperArmH * 0.5f, 0, armW, upperArmH * 0.5f, armW);
    AddBox(mesh, -armX, armY - upperArmH * 0.5f, 0, armW, upperArmH * 0.5f, armW);

    // --- Forearms ---
    float forearmH = 0.14f * s * params.armRatio;
    float forearmY = armY - upperArmH;
    AddBox(mesh,  armX, forearmY - forearmH * 0.5f, 0, armW * 0.85f, forearmH * 0.5f, armW * 0.85f);
    AddBox(mesh, -armX, forearmY - forearmH * 0.5f, 0, armW * 0.85f, forearmH * 0.5f, armW * 0.85f);

    // --- Upper legs ---
    float legW = 0.06f * s * massScale;
    float upperLegH = 0.22f * s * params.legRatio;
    float legX = 0.08f * s;
    float legTopY = torsoY - torsoH;
    AddBox(mesh,  legX, legTopY - upperLegH * 0.5f, 0, legW, upperLegH * 0.5f, legW);
    AddBox(mesh, -legX, legTopY - upperLegH * 0.5f, 0, legW, upperLegH * 0.5f, legW);

    // --- Lower legs ---
    float lowerLegH = 0.20f * s * params.legRatio;
    float lowerLegY = legTopY - upperLegH;
    AddBox(mesh,  legX, lowerLegY - lowerLegH * 0.5f, 0, legW * 0.85f, lowerLegH * 0.5f, legW * 0.85f);
    AddBox(mesh, -legX, lowerLegY - lowerLegH * 0.5f, 0, legW * 0.85f, lowerLegH * 0.5f, legW * 0.85f);

    // --- Feet ---
    float footH = 0.03f * s;
    float footD = 0.08f * s;
    float footY = lowerLegY - lowerLegH;
    AddBox(mesh,  legX, footY - footH, 0.02f * s, legW, footH, footD);
    AddBox(mesh, -legX, footY - footH, 0.02f * s, legW, footH, footD);

    return mesh;
}

procedural::MeshData HumanMeshGenerator::GenerateFromContext(const CharacterContext& ctx) {
    CharacterGraph graph;
    auto bodyId = graph.AddNode(std::make_unique<BaseBodyNode>());
    auto faceId = graph.AddNode(std::make_unique<FacialFeatureNode>());
    auto hairId = graph.AddNode(std::make_unique<HairStyleNode>());
    auto matId  = graph.AddNode(std::make_unique<MaterialNode>());

    graph.Compile();
    graph.Execute(ctx);

    auto params = ParamsFromGraph(graph, bodyId, faceId, hairId, matId);
    return Generate(params);
}

} // namespace atlas::character
