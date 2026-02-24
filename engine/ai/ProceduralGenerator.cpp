#include "ProceduralGenerator.h"
#include <algorithm>
#include <sstream>

namespace atlas::ai {

// ============================================================
// Name fragment tables — one pair (prefixes, suffixes) per culture
// ============================================================

static const std::vector<std::string> kGenericPrefixes  = {"John", "Anna", "Elm", "Oak", "River", "Stone", "Sky", "Dawn"};
static const std::vector<std::string> kGenericSuffixes  = {"son", "ley", "ton", "field", "wood", "vale", "brook", "dale"};

static const std::vector<std::string> kNordicPrefixes   = {"Thor", "Bjorn", "Freya", "Sig", "Ulf", "Rag", "Heid", "Sten"};
static const std::vector<std::string> kNordicSuffixes   = {"heim", "gard", "mund", "rik", "sten", "vald", "borg", "run"};

static const std::vector<std::string> kEasternPrefixes  = {"Kai", "Ren", "Yuki", "Hiro", "Akira", "Sora", "Hana", "Ryu"};
static const std::vector<std::string> kEasternSuffixes  = {"moto", "kawa", "saki", "mura", "yama", "hashi", "guchi", "tani"};

static const std::vector<std::string> kLatinPrefixes    = {"Aug", "Max", "Val", "Luc", "Oct", "Aur", "Cas", "Jul"};
static const std::vector<std::string> kLatinSuffixes    = {"ustus", "imus", "erius", "entius", "anus", "onius", "inus", "illus"};

static const std::vector<std::string> kSciFiPrefixes    = {"Zyx", "Kron", "Vex", "Nyx", "Aur", "Qel", "Drax", "Ion"};
static const std::vector<std::string> kSciFiSuffixes    = {"-7", "-X", "Prime", "Nova", "Core", "-9", "Flux", "Tek"};

static const std::vector<std::string> kFantasyPrefixes  = {"Ael", "Thr", "Zan", "Kael", "Mor", "Fae", "Lyr", "Eld"};
static const std::vector<std::string> kFantasySuffixes  = {"andor", "enthos", "wyn", "ithil", "drim", "oria", "alis", "anwe"};

// Category-specific suffixes
static const std::vector<std::string> kPlaceSuffixes    = {"Falls", "Haven", "Reach", "Deep", "Hollow", "Summit", "Gate", "Crossing"};
static const std::vector<std::string> kItemSuffixes     = {"blade", "stone", "crown", "ring", "shard", "sigil", "tome", "relic"};

// ============================================================
// Constructor
// ============================================================

ProceduralGenerator::ProceduralGenerator() {}

// --- Seed ---

void ProceduralGenerator::SetSeed(uint64_t seed) {
    m_seed = seed;
    m_rngState = seed ? seed : 1;  // xorshift64 requires non-zero state
}

uint64_t ProceduralGenerator::GetSeed() const {
    return m_seed;
}

// --- LLM Backend ---

void ProceduralGenerator::SetLLMBackend(ILLMBackend* backend) {
    m_llmBackend = backend;
}

ILLMBackend* ProceduralGenerator::GetLLMBackend() const {
    return m_llmBackend;
}

// ============================================================
// Deterministic RNG (xorshift64)
// ============================================================

uint32_t ProceduralGenerator::DeterministicRand() {
    m_rngState ^= m_rngState >> 12;
    m_rngState ^= m_rngState << 25;
    m_rngState ^= m_rngState >> 27;
    return static_cast<uint32_t>(m_rngState * 2685821657736338717ULL);
}

std::string ProceduralGenerator::PickRandom(const std::vector<std::string>& options) {
    if (options.empty()) return "";
    return options[DeterministicRand() % options.size()];
}

// ============================================================
// Name Generation
// ============================================================

static const std::vector<std::string>& PrefixesForCulture(NameCulture c) {
    switch (c) {
        case NameCulture::Nordic:  return kNordicPrefixes;
        case NameCulture::Eastern: return kEasternPrefixes;
        case NameCulture::Latin:   return kLatinPrefixes;
        case NameCulture::SciFi:   return kSciFiPrefixes;
        case NameCulture::Fantasy: return kFantasyPrefixes;
        default:                   return kGenericPrefixes;
    }
}

static const std::vector<std::string>& SuffixesForCulture(NameCulture c) {
    switch (c) {
        case NameCulture::Nordic:  return kNordicSuffixes;
        case NameCulture::Eastern: return kEasternSuffixes;
        case NameCulture::Latin:   return kLatinSuffixes;
        case NameCulture::SciFi:   return kSciFiSuffixes;
        case NameCulture::Fantasy: return kFantasySuffixes;
        default:                   return kGenericSuffixes;
    }
}

GeneratedName ProceduralGenerator::GenerateName(const std::string& category,
                                                 NameCulture culture) {
    // Try LLM backend first
    if (m_llmBackend && m_llmBackend->IsAvailable()) {
        LLMRequest req;
        req.prompt = "Generate a single " + category + " name for a game.";
        req.maxTokens = 32;
        auto resp = m_llmBackend->Complete(req);
        if (resp.success && !resp.text.empty()) {
            ++m_generationCount;
            return {resp.text, culture, category};
        }
    }

    // Template fallback
    const auto& prefixes = PrefixesForCulture(culture);
    const auto& suffixes = SuffixesForCulture(culture);

    std::string name = PickRandom(prefixes);

    if (category == "place") {
        name += PickRandom(kPlaceSuffixes);
    } else if (category == "item") {
        name += PickRandom(kItemSuffixes);
    } else {
        name += PickRandom(suffixes);
    }

    ++m_generationCount;
    return {name, culture, category};
}

std::vector<GeneratedName> ProceduralGenerator::GenerateNames(const std::string& category,
                                                               NameCulture culture,
                                                               uint32_t count) {
    std::vector<GeneratedName> results;
    results.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        results.push_back(GenerateName(category, culture));
    }
    return results;
}

// ============================================================
// Biome Asset Suggestions
// ============================================================

BiomeAssetList ProceduralGenerator::GenerateBiomeAssets(BiomeType biome) {
    BiomeAssetList list;
    list.biome = biome;

    switch (biome) {
        case BiomeType::Forest:
            list.meshSuggestions    = {"pine_tree", "oak_tree", "fern", "mushroom", "fallen_log", "bush", "moss_rock"};
            list.textureSuggestions = {"bark_diffuse", "leaf_atlas", "forest_floor", "moss_normal", "fern_alpha"};
            list.vfxSuggestions     = {"firefly_particles", "leaf_fall", "fog_volume", "dust_motes"};
            list.audioSuggestions   = {"bird_chirp", "wind_through_trees", "creek_flow", "rustling_leaves"};
            break;
        case BiomeType::Desert:
            list.meshSuggestions    = {"sand_dune", "cactus", "desert_rock", "skull", "oasis_palm", "sandstone_arch"};
            list.textureSuggestions = {"sand_diffuse", "cracked_earth", "sandstone_normal", "dust_overlay"};
            list.vfxSuggestions     = {"sand_storm", "heat_haze", "dust_devil", "mirage_shimmer"};
            list.audioSuggestions   = {"desert_wind", "sand_shift", "scorpion_click", "distant_thunder"};
            break;
        case BiomeType::Mountain:
            list.meshSuggestions    = {"cliff_face", "boulder", "pine_tree_alpine", "snow_cap", "mountain_goat", "cave_entrance"};
            list.textureSuggestions = {"granite_diffuse", "snow_normal", "cliff_moss", "ice_overlay"};
            list.vfxSuggestions     = {"snow_fall", "avalanche_dust", "cloud_layer", "wind_streak"};
            list.audioSuggestions   = {"mountain_wind", "rock_slide", "eagle_cry", "echo_ambient"};
            break;
        case BiomeType::Ocean:
            list.meshSuggestions    = {"wave_plane", "coral_reef", "seaweed", "fish_school", "shipwreck", "lighthouse"};
            list.textureSuggestions = {"water_normal", "caustics", "foam_mask", "ocean_depth_gradient"};
            list.vfxSuggestions     = {"water_splash", "bubble_rise", "foam_trail", "god_rays_underwater"};
            list.audioSuggestions   = {"ocean_waves", "seagull_cry", "underwater_ambience", "whale_song"};
            break;
        case BiomeType::Tundra:
            list.meshSuggestions    = {"ice_sheet", "frozen_tree", "snow_drift", "ice_crystal", "mammoth_skeleton"};
            list.textureSuggestions = {"ice_diffuse", "snow_sparkle", "frost_overlay", "tundra_ground"};
            list.vfxSuggestions     = {"blizzard", "breath_fog", "ice_crack", "aurora_borealis"};
            list.audioSuggestions   = {"howling_wind", "ice_creak", "snow_crunch", "wolf_howl"};
            break;
        case BiomeType::Swamp:
            list.meshSuggestions    = {"mangrove_tree", "lily_pad", "dead_tree", "swamp_hut", "hanging_moss", "bog_rock"};
            list.textureSuggestions = {"murky_water", "wet_mud", "algae_overlay", "rotten_bark"};
            list.vfxSuggestions     = {"swamp_fog", "bubble_pop", "insect_swarm", "will_o_wisp"};
            list.audioSuggestions   = {"frog_croak", "dripping_water", "insect_buzz", "squelch_footstep"};
            break;
        case BiomeType::Plains:
            list.meshSuggestions    = {"tall_grass", "wildflower", "hay_bale", "windmill", "fence_post", "scarecrow"};
            list.textureSuggestions = {"grass_diffuse", "dirt_path", "wheat_atlas", "cloud_shadow"};
            list.vfxSuggestions     = {"grass_sway", "pollen_drift", "butterfly_swarm", "wind_gust"};
            list.audioSuggestions   = {"wind_grass", "cricket_chirp", "horse_gallop", "distant_bell"};
            break;
        case BiomeType::Volcanic:
            list.meshSuggestions    = {"lava_rock", "volcano_cone", "obsidian_shard", "charred_tree", "magma_pool", "vent"};
            list.textureSuggestions = {"lava_emissive", "basalt_diffuse", "ash_overlay", "scorched_ground"};
            list.vfxSuggestions     = {"lava_flow", "ember_particles", "smoke_column", "volcanic_explosion"};
            list.audioSuggestions   = {"lava_bubble", "rumble_deep", "steam_hiss", "eruption_boom"};
            break;
        case BiomeType::Urban:
            list.meshSuggestions    = {"building_facade", "street_lamp", "bench", "dumpster", "fire_hydrant", "traffic_cone"};
            list.textureSuggestions = {"concrete_diffuse", "brick_normal", "asphalt_wet", "graffiti_decal"};
            list.vfxSuggestions     = {"rain_streak", "neon_glow", "steam_vent", "car_headlight"};
            list.audioSuggestions   = {"traffic_ambient", "crowd_murmur", "siren_distant", "footstep_concrete"};
            break;
    }

    ++m_generationCount;
    return list;
}

std::vector<BiomeAssetList> ProceduralGenerator::GenerateAllBiomeAssets() {
    std::vector<BiomeAssetList> all;
    all.reserve(9);
    for (uint8_t i = 0; i <= static_cast<uint8_t>(BiomeType::Urban); ++i) {
        all.push_back(GenerateBiomeAssets(static_cast<BiomeType>(i)));
    }
    return all;
}

// ============================================================
// Quest Templates
// ============================================================

static const char* QuestTypeLabel(QuestType t) {
    switch (t) {
        case QuestType::Fetch:       return "Fetch";
        case QuestType::Combat:      return "Combat";
        case QuestType::Exploration: return "Exploration";
        case QuestType::Escort:      return "Escort";
        case QuestType::Puzzle:      return "Puzzle";
        case QuestType::Delivery:    return "Delivery";
    }
    return "Unknown";
}

QuestTemplate ProceduralGenerator::GenerateQuest(QuestType type, uint32_t difficulty) {
    difficulty = std::clamp(difficulty, 1u, 5u);

    // Try LLM backend first
    if (m_llmBackend && m_llmBackend->IsAvailable()) {
        LLMRequest req;
        std::ostringstream oss;
        oss << "Generate a " << QuestTypeLabel(type)
            << " quest outline with difficulty " << difficulty
            << "/5. Include title, description, and objectives.";
        req.prompt = oss.str();
        req.maxTokens = 256;
        auto resp = m_llmBackend->Complete(req);
        if (resp.success && !resp.text.empty()) {
            QuestTemplate q;
            q.type = type;
            q.title = resp.text.substr(0, resp.text.find('\n'));
            q.description = resp.text;
            q.estimatedDifficulty = difficulty;
            ++m_generationCount;
            return q;
        }
    }

    // Template-based fallback
    QuestTemplate q;
    q.type = type;
    q.estimatedDifficulty = difficulty;

    static const std::vector<std::string> fetchItems   = {"Ancient Relic", "Healing Herb", "Sacred Scroll", "Crystal Shard", "Dragon Scale"};
    static const std::vector<std::string> enemies       = {"Bandits", "Undead Horde", "Dragon", "Dark Sorcerer", "Giant Spider"};
    static const std::vector<std::string> locations     = {"Forgotten Ruins", "Cursed Cavern", "Misty Peak", "Sunken Temple", "Shadow Forest"};
    static const std::vector<std::string> escorts       = {"Merchant", "Princess", "Scholar", "Wounded Soldier", "Lost Child"};
    static const std::vector<std::string> puzzles       = {"Ancient Lock", "Rune Sequence", "Mirror Maze", "Elemental Altar", "Star Map"};
    static const std::vector<std::string> deliveryItems = {"Royal Decree", "Medicine Bundle", "Enchanted Weapon", "Trade Goods", "Secret Plans"};

    switch (type) {
        case QuestType::Fetch: {
            std::string item = PickRandom(fetchItems);
            std::string loc  = PickRandom(locations);
            q.title = "Retrieve the " + item;
            q.description = "A valuable " + item + " has been lost in the " + loc + ". Venture forth and recover it.";
            q.objectives = {"Travel to the " + loc, "Find the " + item, "Return the " + item + " safely"};
            q.rewardDescription = "Gold and experience";
            break;
        }
        case QuestType::Combat: {
            std::string enemy = PickRandom(enemies);
            std::string loc   = PickRandom(locations);
            q.title = "Defeat the " + enemy;
            q.description = "The " + enemy + " threaten the realm from the " + loc + ". Put an end to their menace.";
            q.objectives = {"Travel to the " + loc, "Defeat the " + enemy, "Collect proof of victory"};
            q.rewardDescription = "Rare equipment and renown";
            break;
        }
        case QuestType::Exploration: {
            std::string loc = PickRandom(locations);
            q.title = "Explore the " + loc;
            q.description = "Little is known about the " + loc + ". Map the area and report your findings.";
            q.objectives = {"Reach the " + loc, "Discover 3 points of interest", "Return with a full map"};
            q.rewardDescription = "Cartography tools and gold";
            break;
        }
        case QuestType::Escort: {
            std::string person = PickRandom(escorts);
            std::string loc    = PickRandom(locations);
            q.title = "Escort the " + person;
            q.description = "The " + person + " must reach the " + loc + " safely. Guard them on the journey.";
            q.objectives = {"Meet the " + person, "Protect them en route to the " + loc, "Ensure safe arrival"};
            q.rewardDescription = "Reputation and a unique item";
            break;
        }
        case QuestType::Puzzle: {
            std::string puzzle = PickRandom(puzzles);
            std::string loc    = PickRandom(locations);
            q.title = "Solve the " + puzzle;
            q.description = "Deep within the " + loc + " lies the " + puzzle + ". Only the wise can unlock its secrets.";
            q.objectives = {"Enter the " + loc, "Decipher the " + puzzle, "Claim the hidden reward"};
            q.rewardDescription = "Ancient knowledge and a rare artifact";
            break;
        }
        case QuestType::Delivery: {
            std::string item = PickRandom(deliveryItems);
            std::string loc  = PickRandom(locations);
            q.title = "Deliver the " + item;
            q.description = "The " + item + " must reach the " + loc + " before time runs out.";
            q.objectives = {"Collect the " + item, "Travel to the " + loc, "Deliver on time"};
            q.rewardDescription = "Gold and faction standing";
            break;
        }
    }

    ++m_generationCount;
    return q;
}

std::vector<QuestTemplate> ProceduralGenerator::GenerateQuestLine(uint32_t count,
                                                                   uint32_t startDifficulty) {
    std::vector<QuestTemplate> line;
    line.reserve(count);

    static const QuestType kTypes[] = {
        QuestType::Fetch, QuestType::Combat, QuestType::Exploration,
        QuestType::Escort, QuestType::Puzzle, QuestType::Delivery
    };
    const uint32_t kTypeCount = static_cast<uint32_t>(std::size(kTypes));

    for (uint32_t i = 0; i < count; ++i) {
        QuestType type = kTypes[DeterministicRand() % kTypeCount];
        uint32_t diff = std::clamp(startDifficulty + i, 1u, 5u);
        line.push_back(GenerateQuest(type, diff));
    }
    return line;
}

// ============================================================
// Shader Suggestions
// ============================================================

std::vector<ShaderSuggestion> ProceduralGenerator::SuggestShaders(BiomeType biome) {
    std::vector<ShaderSuggestion> suggestions;

    switch (biome) {
        case BiomeType::Forest:
            suggestions.push_back({"wind_sway_vegetation", "Vertex animation for foliage wind sway",
                                   {"wind_strength", "wind_direction", "sway_frequency"}, biome});
            suggestions.push_back({"subsurface_leaf", "Subsurface scattering for translucent leaves",
                                   {"thickness", "scattering_color", "light_wrap"}, biome});
            break;
        case BiomeType::Desert:
            suggestions.push_back({"heat_haze", "Screen-space distortion for heat shimmer",
                                   {"distortion_strength", "speed", "height_falloff"}, biome});
            suggestions.push_back({"sand_parallax", "Parallax occlusion mapping for sand dunes",
                                   {"height_scale", "layers", "wind_offset"}, biome});
            break;
        case BiomeType::Mountain:
            suggestions.push_back({"snow_accumulation", "Height-based snow coverage blending",
                                   {"snow_height", "blend_sharpness", "snow_color"}, biome});
            suggestions.push_back({"rock_triplanar", "Triplanar projection for cliff faces",
                                   {"blend_sharpness", "texture_scale", "normal_strength"}, biome});
            break;
        case BiomeType::Ocean:
            suggestions.push_back({"ocean_surface", "FFT-based ocean wave rendering",
                                   {"wave_amplitude", "wind_speed", "choppiness", "foam_threshold"}, biome});
            suggestions.push_back({"underwater_caustics", "Animated caustic light patterns",
                                   {"caustic_scale", "speed", "intensity"}, biome});
            break;
        case BiomeType::Tundra:
            suggestions.push_back({"ice_refraction", "Refraction and subsurface for thick ice",
                                   {"refraction_index", "thickness", "tint_color"}, biome});
            suggestions.push_back({"blizzard_particle", "GPU particle shader for blizzard snow",
                                   {"density", "wind_force", "turbulence"}, biome});
            break;
        case BiomeType::Swamp:
            suggestions.push_back({"murky_water", "Dark water with depth-based absorption",
                                   {"absorption_color", "clarity", "ripple_scale"}, biome});
            suggestions.push_back({"fog_volume", "Volumetric fog for low-lying areas",
                                   {"density", "height_falloff", "scatter_color"}, biome});
            break;
        case BiomeType::Plains:
            suggestions.push_back({"grass_blade", "Geometry-shader grass blade renderer",
                                   {"blade_height", "density", "wind_strength", "color_variation"}, biome});
            suggestions.push_back({"cloud_shadow", "Projected cloud shadow map on terrain",
                                   {"shadow_texture", "speed", "opacity"}, biome});
            break;
        case BiomeType::Volcanic:
            suggestions.push_back({"lava_flow", "Animated emissive lava with flow mapping",
                                   {"flow_speed", "glow_intensity", "crust_threshold"}, biome});
            suggestions.push_back({"heat_distortion", "Near-surface heat distortion effect",
                                   {"distortion_amount", "rise_speed", "height_range"}, biome});
            break;
        case BiomeType::Urban:
            suggestions.push_back({"puddle_reflection", "Screen-space puddle reflections on wet ground",
                                   {"wetness", "roughness_override", "puddle_mask"}, biome});
            suggestions.push_back({"neon_emissive", "Animated neon sign emissive shader",
                                   {"glow_color", "flicker_rate", "intensity"}, biome});
            break;
    }

    ++m_generationCount;
    return suggestions;
}

ShaderSuggestion ProceduralGenerator::SuggestShader(const std::string& environmentType) {
    // Map string to BiomeType for lookup
    struct Mapping { const char* key; BiomeType biome; };
    static const Mapping kMappings[] = {
        {"forest",   BiomeType::Forest},
        {"desert",   BiomeType::Desert},
        {"mountain", BiomeType::Mountain},
        {"ocean",    BiomeType::Ocean},
        {"tundra",   BiomeType::Tundra},
        {"swamp",    BiomeType::Swamp},
        {"plains",   BiomeType::Plains},
        {"volcanic", BiomeType::Volcanic},
        {"urban",    BiomeType::Urban},
    };

    for (const auto& m : kMappings) {
        if (environmentType == m.key) {
            auto list = SuggestShaders(m.biome);
            if (!list.empty()) {
                return list[DeterministicRand() % list.size()];
            }
        }
    }

    // Fallback generic suggestion
    ++m_generationCount;
    return {"pbr_standard", "Standard PBR metallic-roughness shader",
            {"albedo", "metallic", "roughness", "normal"}, BiomeType::Plains};
}

// --- Statistics ---

uint32_t ProceduralGenerator::GetGenerationCount() const {
    return m_generationCount;
}

void ProceduralGenerator::ResetStatistics() {
    m_generationCount = 0;
}

} // namespace atlas::ai
