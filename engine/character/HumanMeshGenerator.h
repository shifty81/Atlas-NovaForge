#pragma once
#include "CharacterGraph.h"
#include "CharacterNodes.h"
#include "../procedural/ProceduralMeshGraph.h"
#include <cstdint>
#include <string>

namespace atlas::character {

/// Parameters that control procedural human mesh generation.
/// Values are typically produced by evaluating a CharacterGraph
/// but may also be edited directly through the CharacterEditorPanel.
struct HumanMeshParams {
    // Body proportions (from BaseBodyNode)
    float height    = 1.8f;
    float mass      = 70.0f;
    float armRatio  = 1.0f;
    float legRatio  = 1.0f;

    // Facial features (from FacialFeatureNode)
    float noseWidth   = 0.5f;
    float eyeSpacing  = 0.55f;
    float jawWidth    = 0.7f;
    float browHeight  = 0.5f;
    float lipFullness = 0.5f;

    // Hair style (from HairStyleNode)
    float hairStyle   = 0.0f;
    float hairLength  = 0.5f;
    float hairDensity = 0.7f;
    float hairR = 0.4f, hairG = 0.3f, hairB = 0.2f;

    // Material (from MaterialNode)
    float skinTone  = 0.5f;
    float hairColor = 0.5f;
    float eyeColor  = 0.5f;
};

/// Generates a procedural human mesh from HumanMeshParams.
/// The output is a MeshData suitable for rendering in the
/// MeshViewerPanel or the CharacterSelectScreen viewport.
class HumanMeshGenerator {
public:
    /// Build a HumanMeshParams from a fully-evaluated CharacterGraph.
    /// The graph must already have been compiled and executed.
    static HumanMeshParams ParamsFromGraph(
        const CharacterGraph& graph,
        CharacterNodeID bodyNode,
        CharacterNodeID faceNode,
        CharacterNodeID hairNode,
        CharacterNodeID materialNode);

    /// Generate a complete human mesh from the given parameters.
    static procedural::MeshData Generate(const HumanMeshParams& params);

    /// Convenience: build a default CharacterGraph, execute it with
    /// the given context, and return the resulting mesh.
    static procedural::MeshData GenerateFromContext(const CharacterContext& ctx);
};

} // namespace atlas::character
