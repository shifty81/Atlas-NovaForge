#include "pcg/lowpoly_character_generator.h"
#include <sstream>

namespace atlas {
namespace pcg {

// ── Palette region index constants ──────────────────────────────────
static constexpr int SKIN_PALETTE_IDX  = 0;
static constexpr int HAIR_PALETTE_IDX  = 1;
static constexpr int SHIRT_PALETTE_IDX = 2;
static constexpr int PANTS_PALETTE_IDX = 3;

// ── Body part index constants (order matches buildBodyParts) ────────
static constexpr int BODY_IDX_HEAD      = 0;
static constexpr int BODY_IDX_TORSO     = 1;
static constexpr int BODY_IDX_ARM_LEFT  = 2;
static constexpr int BODY_IDX_ARM_RIGHT = 3;
static constexpr int BODY_IDX_LEG_LEFT  = 4;
static constexpr int BODY_IDX_LEG_RIGHT = 5;
static constexpr int BODY_IDX_HANDS     = 6;
static constexpr int BODY_IDX_FEET      = 7;

// ── Name helpers ────────────────────────────────────────────────────

const char* bodySlotName(BodySlot slot) {
    switch (slot) {
        case BodySlot::Head:     return "Head";
        case BodySlot::Torso:    return "Torso";
        case BodySlot::ArmLeft:  return "ArmLeft";
        case BodySlot::ArmRight: return "ArmRight";
        case BodySlot::LegLeft:  return "LegLeft";
        case BodySlot::LegRight: return "LegRight";
        case BodySlot::Hands:    return "Hands";
        case BodySlot::Feet:     return "Feet";
    }
    return "Unknown";
}

const char* clothingSlotName(ClothingSlot slot) {
    switch (slot) {
        case ClothingSlot::Hat:           return "Hat";
        case ClothingSlot::Jacket:        return "Jacket";
        case ClothingSlot::Shirt:         return "Shirt";
        case ClothingSlot::Pants:         return "Pants";
        case ClothingSlot::Shoes:         return "Shoes";
        case ClothingSlot::Gloves:        return "Gloves";
        case ClothingSlot::Backpack:      return "Backpack";
        case ClothingSlot::FaceAccessory: return "FaceAccessory";
    }
    return "Unknown";
}

const char* archetypeName(CharacterArchetype archetype) {
    switch (archetype) {
        case CharacterArchetype::Survivor:  return "Survivor";
        case CharacterArchetype::Militia:   return "Militia";
        case CharacterArchetype::Civilian:  return "Civilian";
        case CharacterArchetype::Scavenger: return "Scavenger";
        case CharacterArchetype::Medic:     return "Medic";
    }
    return "Unknown";
}

// ── Mesh variant tables ─────────────────────────────────────────────
// Each body slot has a small set of low-poly mesh variants.  At
// generation time the RNG picks one per slot.

struct MeshVariant {
    const char* file;
    const char* variant;
};

static const MeshVariant HEAD_VARIANTS[] = {
    {"Head_Round.obj",    "round"},
    {"Head_Square.obj",   "square"},
    {"Head_Angular.obj",  "angular"},
};
static constexpr int HEAD_VARIANT_COUNT = 3;

static const MeshVariant TORSO_VARIANTS[] = {
    {"Torso_Standard.obj", "standard"},
    {"Torso_Broad.obj",    "broad"},
    {"Torso_Slim.obj",     "slim"},
};
static constexpr int TORSO_VARIANT_COUNT = 3;

static const MeshVariant ARM_VARIANTS[] = {
    {"Arm_Standard.obj", "standard"},
    {"Arm_Muscular.obj", "muscular"},
    {"Arm_Thin.obj",     "thin"},
};
static constexpr int ARM_VARIANT_COUNT = 3;

static const MeshVariant LEG_VARIANTS[] = {
    {"Leg_Standard.obj", "standard"},
    {"Leg_Long.obj",     "long"},
    {"Leg_Short.obj",    "short"},
};
static constexpr int LEG_VARIANT_COUNT = 3;

static const MeshVariant HAND_VARIANTS[] = {
    {"Hands_Default.obj",  "default"},
    {"Hands_Gloved.obj",   "gloved"},
};
static constexpr int HAND_VARIANT_COUNT = 2;

static const MeshVariant FEET_VARIANTS[] = {
    {"Feet_Boots.obj",   "boots"},
    {"Feet_Shoes.obj",   "shoes"},
    {"Feet_Sandals.obj", "sandals"},
};
static constexpr int FEET_VARIANT_COUNT = 3;

// ── Clothing variant tables ─────────────────────────────────────────
// Clothing is archetype-weighted but ultimately picked by the RNG.

struct ClothingVariant {
    const char* file;
    const char* variant;
    float       survivalWeight;  // Likelihood for Survivor/Scavenger.
    float       civilianWeight;  // Likelihood for Civilian/Medic.
    float       militiaWeight;   // Likelihood for Militia.
};

static const ClothingVariant HAT_VARIANTS[] = {
    {"Hat_Beanie.obj",   "beanie",   0.30f, 0.20f, 0.10f},
    {"Hat_Cap.obj",      "cap",      0.25f, 0.30f, 0.20f},
    {"Hat_Helmet.obj",   "helmet",   0.15f, 0.05f, 0.50f},
    {"Hat_None.obj",     "none",     0.30f, 0.45f, 0.20f},
};
static constexpr int HAT_VARIANT_COUNT = 4;

static const ClothingVariant JACKET_VARIANTS[] = {
    {"Jacket_Hoodie.obj",  "hoodie",  0.35f, 0.30f, 0.10f},
    {"Jacket_Vest.obj",    "vest",    0.30f, 0.15f, 0.35f},
    {"Jacket_Coat.obj",    "coat",    0.20f, 0.25f, 0.15f},
    {"Jacket_None.obj",    "none",    0.15f, 0.30f, 0.40f},
};
static constexpr int JACKET_VARIANT_COUNT = 4;

static const ClothingVariant BACKPACK_VARIANTS[] = {
    {"Backpack_Small.obj",  "small",  0.30f, 0.15f, 0.25f},
    {"Backpack_Large.obj",  "large",  0.25f, 0.05f, 0.30f},
    {"Backpack_None.obj",   "none",   0.45f, 0.80f, 0.45f},
};
static constexpr int BACKPACK_VARIANT_COUNT = 3;

// ── Palette definitions ─────────────────────────────────────────────
// SurrounDead-style: a tiny color atlas per region.

static const PaletteColor SKIN_TONES[] = {
    {0.96f, 0.84f, 0.72f},  // Fair
    {0.87f, 0.72f, 0.53f},  // Light
    {0.76f, 0.57f, 0.38f},  // Medium
    {0.55f, 0.38f, 0.24f},  // Tan
    {0.36f, 0.24f, 0.14f},  // Dark
};
static constexpr int SKIN_TONE_COUNT = 5;

static const PaletteColor HAIR_COLORS[] = {
    {0.10f, 0.07f, 0.05f},  // Black
    {0.35f, 0.20f, 0.10f},  // Brown
    {0.75f, 0.55f, 0.30f},  // Blonde
    {0.60f, 0.15f, 0.10f},  // Red
    {0.50f, 0.50f, 0.50f},  // Grey
};
static constexpr int HAIR_COLOR_COUNT = 5;

static const PaletteColor SHIRT_COLORS[] = {
    {0.30f, 0.30f, 0.30f},  // Charcoal
    {0.20f, 0.35f, 0.20f},  // Olive
    {0.50f, 0.40f, 0.30f},  // Khaki
    {0.15f, 0.15f, 0.40f},  // Navy
    {0.60f, 0.55f, 0.50f},  // Tan
    {0.80f, 0.78f, 0.75f},  // Off-white
};
static constexpr int SHIRT_COLOR_COUNT = 6;

static const PaletteColor PANTS_COLORS[] = {
    {0.15f, 0.15f, 0.15f},  // Black
    {0.25f, 0.30f, 0.20f},  // OD Green
    {0.40f, 0.35f, 0.25f},  // Tan
    {0.20f, 0.20f, 0.35f},  // Dark blue
    {0.35f, 0.30f, 0.25f},  // Brown
};
static constexpr int PANTS_COLOR_COUNT = 5;

// ── Internal helpers ────────────────────────────────────────────────

static LowPolyMeshPiece makePiece(const MeshVariant& mv, int paletteIdx,
                                    float offX, float offY, float offZ) {
    LowPolyMeshPiece piece{};
    piece.meshFile       = mv.file;
    piece.variant        = mv.variant;
    piece.paletteIndex   = paletteIdx;
    piece.scaleX = piece.scaleY = piece.scaleZ = 1.0f;
    piece.offsetX = offX;
    piece.offsetY = offY;
    piece.offsetZ = offZ;
    piece.flatShaded     = true;
    piece.useVertexColors = true;
    return piece;
}

static int weightedPick(DeterministicRNG& rng, const float* weights, int count) {
    float total = 0.0f;
    for (int i = 0; i < count; ++i) total += weights[i];
    float r = rng.rangeFloat(0.0f, total);
    float cumulative = 0.0f;
    for (int i = 0; i < count; ++i) {
        cumulative += weights[i];
        if (r <= cumulative) return i;
    }
    return count - 1;
}

static float archetypeWeight(const ClothingVariant& cv,
                              CharacterArchetype archetype) {
    switch (archetype) {
        case CharacterArchetype::Survivor:
        case CharacterArchetype::Scavenger:
            return cv.survivalWeight;
        case CharacterArchetype::Militia:
            return cv.militiaWeight;
        case CharacterArchetype::Civilian:
        case CharacterArchetype::Medic:
            return cv.civilianWeight;
    }
    return cv.civilianWeight;
}

// ── Generator implementation ────────────────────────────────────────

const char* LowPolyCharacterGenerator::archetypeClassName(CharacterArchetype a) {
    return archetypeName(a);
}

CharacterArchetype LowPolyCharacterGenerator::selectArchetype(DeterministicRNG& rng) {
    return static_cast<CharacterArchetype>(rng.range(0, ARCHETYPE_COUNT - 1));
}

void LowPolyCharacterGenerator::buildBodyParts(DeterministicRNG& rng,
                                                 GeneratedLowPolyCharacter& ch) {
    // Each body slot: pick a random variant and assign a position
    // offset matching the base skeleton layout.
    // Parts are inserted in BODY_IDX_* order — do not reorder.

    int hi = rng.range(0, HEAD_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(HEAD_VARIANTS[hi], SKIN_PALETTE_IDX,
                                      0.0f, 1.60f, 0.0f));

    int ti = rng.range(0, TORSO_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(TORSO_VARIANTS[ti], SKIN_PALETTE_IDX,
                                      0.0f, 1.10f, 0.0f));

    int ali = rng.range(0, ARM_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(ARM_VARIANTS[ali], SKIN_PALETTE_IDX,
                                      -0.35f, 1.20f, 0.0f));
    int ari = rng.range(0, ARM_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(ARM_VARIANTS[ari], SKIN_PALETTE_IDX,
                                      0.35f, 1.20f, 0.0f));

    int lli = rng.range(0, LEG_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(LEG_VARIANTS[lli], SKIN_PALETTE_IDX,
                                      -0.12f, 0.50f, 0.0f));
    int lri = rng.range(0, LEG_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(LEG_VARIANTS[lri], SKIN_PALETTE_IDX,
                                      0.12f, 0.50f, 0.0f));

    int hdi = rng.range(0, HAND_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(HAND_VARIANTS[hdi], SKIN_PALETTE_IDX,
                                      0.0f, 0.85f, 0.0f));

    int fi = rng.range(0, FEET_VARIANT_COUNT - 1);
    ch.bodyParts.push_back(makePiece(FEET_VARIANTS[fi], SKIN_PALETTE_IDX,
                                      0.0f, 0.05f, 0.0f));
}

void LowPolyCharacterGenerator::buildClothing(DeterministicRNG& rng,
                                                GeneratedLowPolyCharacter& ch) {
    // Shirt — always present
    LowPolyMeshPiece shirt{};
    shirt.meshFile   = "Shirt_Default.obj";
    shirt.variant    = "default";
    shirt.paletteIndex = SHIRT_PALETTE_IDX;
    shirt.scaleX = shirt.scaleY = shirt.scaleZ = 1.0f;
    shirt.offsetX = 0.0f; shirt.offsetY = 1.10f; shirt.offsetZ = 0.0f;
    shirt.flatShaded = true;
    shirt.useVertexColors = true;
    ch.clothing.push_back(shirt);

    // Pants — always present
    LowPolyMeshPiece pants{};
    pants.meshFile   = "Pants_Default.obj";
    pants.variant    = "default";
    pants.paletteIndex = PANTS_PALETTE_IDX;
    pants.scaleX = pants.scaleY = pants.scaleZ = 1.0f;
    pants.offsetX = 0.0f; pants.offsetY = 0.50f; pants.offsetZ = 0.0f;
    pants.flatShaded = true;
    pants.useVertexColors = true;
    ch.clothing.push_back(pants);

    // Hat — archetype-weighted
    {
        float weights[HAT_VARIANT_COUNT];
        for (int i = 0; i < HAT_VARIANT_COUNT; ++i)
            weights[i] = archetypeWeight(HAT_VARIANTS[i], ch.archetype);
        int idx = weightedPick(rng, weights, HAT_VARIANT_COUNT);
        auto& hv = HAT_VARIANTS[idx];
        if (std::string(hv.variant) != "none") {
            ch.clothing.push_back(makePiece(
                {hv.file, hv.variant}, HAIR_PALETTE_IDX, 0.0f, 1.72f, 0.0f));
        }
    }

    // Jacket — archetype-weighted
    {
        float weights[JACKET_VARIANT_COUNT];
        for (int i = 0; i < JACKET_VARIANT_COUNT; ++i)
            weights[i] = archetypeWeight(JACKET_VARIANTS[i], ch.archetype);
        int idx = weightedPick(rng, weights, JACKET_VARIANT_COUNT);
        auto& jv = JACKET_VARIANTS[idx];
        if (std::string(jv.variant) != "none") {
            ch.clothing.push_back(makePiece(
                {jv.file, jv.variant}, SHIRT_PALETTE_IDX, 0.0f, 1.15f, 0.0f));
        }
    }

    // Backpack — archetype-weighted
    {
        float weights[BACKPACK_VARIANT_COUNT];
        for (int i = 0; i < BACKPACK_VARIANT_COUNT; ++i)
            weights[i] = archetypeWeight(BACKPACK_VARIANTS[i], ch.archetype);
        int idx = weightedPick(rng, weights, BACKPACK_VARIANT_COUNT);
        auto& bv = BACKPACK_VARIANTS[idx];
        if (std::string(bv.variant) != "none") {
            ch.clothing.push_back(makePiece(
                {bv.file, bv.variant}, PANTS_PALETTE_IDX, 0.0f, 1.25f, -0.15f));
        }
    }
}

void LowPolyCharacterGenerator::buildPalette(DeterministicRNG& rng,
                                               GeneratedLowPolyCharacter& ch) {
    // Region 0: skin
    {
        PaletteRegion region;
        region.regionName = "skin";
        for (int i = 0; i < SKIN_TONE_COUNT; ++i)
            region.colors.push_back(SKIN_TONES[i]);
        region.chosen = rng.range(0, SKIN_TONE_COUNT - 1);
        ch.palette.push_back(region);
    }
    // Region 1: hair
    {
        PaletteRegion region;
        region.regionName = "hair";
        for (int i = 0; i < HAIR_COLOR_COUNT; ++i)
            region.colors.push_back(HAIR_COLORS[i]);
        region.chosen = rng.range(0, HAIR_COLOR_COUNT - 1);
        ch.palette.push_back(region);
    }
    // Region 2: shirt
    {
        PaletteRegion region;
        region.regionName = "shirt";
        for (int i = 0; i < SHIRT_COLOR_COUNT; ++i)
            region.colors.push_back(SHIRT_COLORS[i]);
        region.chosen = rng.range(0, SHIRT_COLOR_COUNT - 1);
        ch.palette.push_back(region);
    }
    // Region 3: pants
    {
        PaletteRegion region;
        region.regionName = "pants";
        for (int i = 0; i < PANTS_COLOR_COUNT; ++i)
            region.colors.push_back(PANTS_COLORS[i]);
        region.chosen = rng.range(0, PANTS_COLOR_COUNT - 1);
        ch.palette.push_back(region);
    }
}

void LowPolyCharacterGenerator::buildFPSArms(DeterministicRNG& rng,
                                               GeneratedLowPolyCharacter& ch) {
    // Find the arm body-part variants that were selected
    std::string leftArm  = "Arm_Standard.obj";
    std::string rightArm = "Arm_Standard.obj";
    if (static_cast<int>(ch.bodyParts.size()) > BODY_IDX_ARM_RIGHT) {
        leftArm  = ch.bodyParts[BODY_IDX_ARM_LEFT].meshFile;
        rightArm = ch.bodyParts[BODY_IDX_ARM_RIGHT].meshFile;
    }

    ch.fpsArms.leftArmMesh      = "FPS_" + leftArm;
    ch.fpsArms.rightArmMesh     = "FPS_" + rightArm;
    ch.fpsArms.skinPaletteIndex = SKIN_PALETTE_IDX;

    // Gloves — match the hand variant
    bool hasGloves = false;
    if (static_cast<int>(ch.bodyParts.size()) > BODY_IDX_HANDS &&
        ch.bodyParts[BODY_IDX_HANDS].variant == "gloved") {
        hasGloves = true;
    }

    if (hasGloves) {
        ch.fpsArms.leftGloveMesh  = "FPS_Glove_L.obj";
        ch.fpsArms.rightGloveMesh = "FPS_Glove_R.obj";
        ch.fpsArms.glovePaletteIndex = PANTS_PALETTE_IDX;
    } else {
        ch.fpsArms.leftGloveMesh.clear();
        ch.fpsArms.rightGloveMesh.clear();
        ch.fpsArms.glovePaletteIndex = SKIN_PALETTE_IDX;
    }

    // Sleeves — present if jacket was selected
    bool hasJacket = false;
    for (const auto& c : ch.clothing) {
        if (c.meshFile.find("Jacket") != std::string::npos &&
            c.variant != "none") {
            hasJacket = true;
            break;
        }
    }

    if (hasJacket) {
        ch.fpsArms.leftSleeveMesh   = "FPS_Sleeve_L.obj";
        ch.fpsArms.rightSleeveMesh  = "FPS_Sleeve_R.obj";
        ch.fpsArms.sleevePaletteIndex = SHIRT_PALETTE_IDX;
    } else {
        ch.fpsArms.leftSleeveMesh.clear();
        ch.fpsArms.rightSleeveMesh.clear();
        ch.fpsArms.sleevePaletteIndex = SKIN_PALETTE_IDX;
    }

    (void)rng; // May be used for future variation
}

bool LowPolyCharacterGenerator::validatePolyBudget(
        const GeneratedLowPolyCharacter& ch) {
    // With < 500 tris per piece and at most ~13 pieces
    // (8 body + 5 clothing) the total stays under 6500.
    int totalPieces = static_cast<int>(ch.bodyParts.size() +
                                       ch.clothing.size());
    return totalPieces > 0 && totalPieces <= 16 &&
           ch.maxTriCount <= 8000;
}

// ── Public generate entry points ────────────────────────────────────

GeneratedLowPolyCharacter LowPolyCharacterGenerator::generate(
        const PCGContext& ctx) {
    DeterministicRNG rng(ctx.seed);
    CharacterArchetype archetype = selectArchetype(rng);
    bool isMale = rng.chance(0.5f);

    GeneratedLowPolyCharacter ch{};
    ch.characterId     = ctx.seed;
    ch.archetype       = archetype;
    ch.isMale          = isMale;
    ch.flatShaded      = true;
    ch.useVertexColors = true;
    ch.maxTriCount     = 6500;
    ch.skeletonId      = "base_humanoid";

    buildPalette(rng, ch);
    buildBodyParts(rng, ch);
    buildClothing(rng, ch);
    buildFPSArms(rng, ch);

    ch.valid = validatePolyBudget(ch);
    return ch;
}

GeneratedLowPolyCharacter LowPolyCharacterGenerator::generate(
        const PCGContext& ctx, CharacterArchetype archetype) {
    DeterministicRNG rng(ctx.seed);
    (void)rng.nextU32(); // consume what selectArchetype would
    bool isMale = rng.chance(0.5f);

    GeneratedLowPolyCharacter ch{};
    ch.characterId     = ctx.seed;
    ch.archetype       = archetype;
    ch.isMale          = isMale;
    ch.flatShaded      = true;
    ch.useVertexColors = true;
    ch.maxTriCount     = 6500;
    ch.skeletonId      = "base_humanoid";

    buildPalette(rng, ch);
    buildBodyParts(rng, ch);
    buildClothing(rng, ch);
    buildFPSArms(rng, ch);

    ch.valid = validatePolyBudget(ch);
    return ch;
}

GeneratedLowPolyCharacter LowPolyCharacterGenerator::generate(
        const PCGContext& ctx, CharacterArchetype archetype, bool isMale) {
    DeterministicRNG rng(ctx.seed);
    (void)rng.nextU32(); // consume selectArchetype slot
    (void)rng.nextU32(); // consume gender slot

    GeneratedLowPolyCharacter ch{};
    ch.characterId     = ctx.seed;
    ch.archetype       = archetype;
    ch.isMale          = isMale;
    ch.flatShaded      = true;
    ch.useVertexColors = true;
    ch.maxTriCount     = 6500;
    ch.skeletonId      = "base_humanoid";

    buildPalette(rng, ch);
    buildBodyParts(rng, ch);
    buildClothing(rng, ch);
    buildFPSArms(rng, ch);

    ch.valid = validatePolyBudget(ch);
    return ch;
}

} // namespace pcg
} // namespace atlas
