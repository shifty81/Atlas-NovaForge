#include "ProjectContext.h"
#include <algorithm>
#include <sstream>

namespace atlas::ai {

// Confidence constants for asset suggestions
constexpr double REQUIREMENT_BASED_CONFIDENCE = 0.8;

ProjectContext::ProjectContext() {}

// --- Game Type ---

void ProjectContext::SetGameType(GameType type) {
    m_gameType = type;
}

GameType ProjectContext::GetGameType() const {
    return m_gameType;
}

void ProjectContext::DetectGameType() {
    // Heuristic detection based on asset inventory
    auto& counts = m_inventory.categoryCounts;
    
    // If we have significant 2D assets, likely platformer or puzzle
    uint32_t mesh2dCount = counts.count(AssetCategory::Mesh2D) ? counts[AssetCategory::Mesh2D] : 0;
    uint32_t mesh3dCount = counts.count(AssetCategory::Mesh3D) ? counts[AssetCategory::Mesh3D] : 0;
    uint32_t characterCount = counts.count(AssetCategory::Character) ? counts[AssetCategory::Character] : 0;
    uint32_t npcCount = counts.count(AssetCategory::NPC) ? counts[AssetCategory::NPC] : 0;
    uint32_t terrainCount = counts.count(AssetCategory::Terrain) ? counts[AssetCategory::Terrain] : 0;
    
    if (mesh2dCount > mesh3dCount * 2) {
        m_gameType = GameType::Platformer;
    } else if (characterCount > 5 && npcCount > 10) {
        m_gameType = GameType::RPG;
    } else if (terrainCount > 3 && mesh3dCount > 20) {
        m_gameType = GameType::Adventure;
    } else if (mesh3dCount > 0) {
        m_gameType = GameType::Simulation;
    } else {
        m_gameType = GameType::Unknown;
    }
}

// --- Project Info ---

void ProjectContext::SetProjectName(const std::string& name) {
    m_projectName = name;
}

const std::string& ProjectContext::GetProjectName() const {
    return m_projectName;
}

void ProjectContext::SetProjectRoot(const std::string& root) {
    m_projectRoot = root;
}

const std::string& ProjectContext::GetProjectRoot() const {
    return m_projectRoot;
}

// --- Asset Inventory ---

void ProjectContext::ScanAssets() {
    // In a full implementation, this would scan the project directory
    // For now, this is a placeholder that can be called after AddAsset
    UpdateRequirementsFulfillment();
}

void ProjectContext::AddAsset(const std::string& id, AssetCategory category) {
    m_inventory.assetIds.insert(id);
    m_inventory.categoryCounts[category]++;
    m_inventory.totalAssetCount++;
    m_assetCategories[id] = category;
}

void ProjectContext::RemoveAsset(const std::string& id) {
    auto it = m_assetCategories.find(id);
    if (it != m_assetCategories.end()) {
        AssetCategory category = it->second;
        m_inventory.assetIds.erase(id);
        if (m_inventory.categoryCounts[category] > 0) {
            m_inventory.categoryCounts[category]--;
        }
        if (m_inventory.totalAssetCount > 0) {
            m_inventory.totalAssetCount--;
        }
        m_assetCategories.erase(it);
    }
}

bool ProjectContext::HasAsset(const std::string& id) const {
    return m_inventory.assetIds.find(id) != m_inventory.assetIds.end();
}

bool ProjectContext::HasAssetCategory(AssetCategory category) const {
    auto it = m_inventory.categoryCounts.find(category);
    return it != m_inventory.categoryCounts.end() && it->second > 0;
}

uint32_t ProjectContext::GetAssetCount(AssetCategory category) const {
    auto it = m_inventory.categoryCounts.find(category);
    return it != m_inventory.categoryCounts.end() ? it->second : 0;
}

const AssetInventory& ProjectContext::GetInventory() const {
    return m_inventory;
}

// --- Requirements ---

void ProjectContext::AddRequirement(const AssetRequirement& req) {
    m_requirements.push_back(req);
}

void ProjectContext::ClearRequirements() {
    m_requirements.clear();
}

void ProjectContext::AnalyzeRequirements() {
    m_requirements = GenerateDefaultRequirements(m_gameType);
    UpdateRequirementsFulfillment();
}

const std::vector<AssetRequirement>& ProjectContext::GetRequirements() const {
    return m_requirements;
}

std::vector<AssetRequirement> ProjectContext::GetUnfulfilledRequirements() const {
    std::vector<AssetRequirement> unfulfilled;
    for (const auto& req : m_requirements) {
        if (!req.fulfilled) {
            unfulfilled.push_back(req);
        }
    }
    return unfulfilled;
}

void ProjectContext::UpdateRequirementsFulfillment() {
    for (auto& req : m_requirements) {
        req.fulfilled = HasAssetCategory(req.category);
        req.matchingAssetIds.clear();
        
        // Find matching assets
        for (const auto& kv : m_assetCategories) {
            if (kv.second == req.category) {
                req.matchingAssetIds.push_back(kv.first);
            }
        }
    }
}

std::vector<AssetRequirement> ProjectContext::GenerateDefaultRequirements(GameType type) const {
    std::vector<AssetRequirement> reqs;
    
    // Common requirements for all game types
    reqs.push_back({AssetCategory::UI, "ui_elements", "User interface elements", 1, false, {}});
    reqs.push_back({AssetCategory::Audio, "sound_effects", "Basic sound effects", 1, false, {}});
    
    switch (type) {
        case GameType::RPG:
            reqs.push_back({AssetCategory::Character, "player_character", "Main player character model", 3, false, {}});
            reqs.push_back({AssetCategory::NPC, "npc_models", "NPC character models", 2, false, {}});
            reqs.push_back({AssetCategory::Terrain, "terrain_textures", "Terrain and ground textures", 2, false, {}});
            reqs.push_back({AssetCategory::Environment, "environment_assets", "Trees, rocks, buildings", 2, false, {}});
            reqs.push_back({AssetCategory::Animation, "character_animations", "Walk, run, idle animations", 2, false, {}});
            reqs.push_back({AssetCategory::Music, "background_music", "Ambient and battle music", 1, false, {}});
            break;
            
        case GameType::FPS:
        case GameType::TPS:
            reqs.push_back({AssetCategory::Character, "player_character", "First-person arms or full character", 3, false, {}});
            reqs.push_back({AssetCategory::Mesh3D, "weapons", "Weapon models", 3, false, {}});
            reqs.push_back({AssetCategory::VFX, "muzzle_flash", "Weapon visual effects", 2, false, {}});
            reqs.push_back({AssetCategory::Environment, "level_geometry", "Level and map assets", 2, false, {}});
            break;
            
        case GameType::Strategy:
            reqs.push_back({AssetCategory::Mesh3D, "unit_models", "Unit models for army", 3, false, {}});
            reqs.push_back({AssetCategory::Terrain, "terrain_tiles", "Terrain tile textures", 2, false, {}});
            reqs.push_back({AssetCategory::UI, "strategy_ui", "Resource bars, minimaps", 2, false, {}});
            break;
            
        case GameType::Platformer:
            reqs.push_back({AssetCategory::Mesh2D, "character_sprites", "Player character sprites", 3, false, {}});
            reqs.push_back({AssetCategory::Mesh2D, "tileset", "Platform and level tiles", 3, false, {}});
            reqs.push_back({AssetCategory::Mesh2D, "background", "Background layers", 2, false, {}});
            reqs.push_back({AssetCategory::Animation, "sprite_animations", "Character animation frames", 2, false, {}});
            break;
            
        case GameType::Puzzle:
            reqs.push_back({AssetCategory::Mesh2D, "puzzle_pieces", "Puzzle element sprites", 3, false, {}});
            reqs.push_back({AssetCategory::VFX, "success_effects", "Completion visual effects", 1, false, {}});
            break;
            
        case GameType::Simulation:
            reqs.push_back({AssetCategory::Mesh3D, "simulation_objects", "Simulated entity models", 2, false, {}});
            reqs.push_back({AssetCategory::Terrain, "world_terrain", "World terrain data", 2, false, {}});
            break;
            
        default:
            // For unknown/custom, add minimal requirements
            reqs.push_back({AssetCategory::Mesh3D, "basic_meshes", "Basic 3D models", 1, false, {}});
            break;
    }
    
    return reqs;
}

// --- Suggestions ---

std::vector<AssetSuggestion> ProjectContext::GenerateSuggestions() const {
    std::vector<AssetSuggestion> suggestions;
    
    // Generate suggestions based on unfulfilled requirements
    for (const auto& req : m_requirements) {
        if (!req.fulfilled) {
            AssetSuggestion suggestion;
            suggestion.category = req.category;
            suggestion.name = req.name;
            suggestion.description = req.description;
            suggestion.priority = req.priority;
            suggestion.confidence = REQUIREMENT_BASED_CONFIDENCE;
            
            // Suggest source based on asset category
            switch (req.category) {
                case AssetCategory::Mesh3D:
                case AssetCategory::Character:
                case AssetCategory::NPC:
                case AssetCategory::Environment:
                    suggestion.source = "marketplace:sketchfab";
                    break;
                case AssetCategory::Mesh2D:
                case AssetCategory::Texture:
                    suggestion.source = "marketplace:itch.io";
                    break;
                case AssetCategory::Terrain:
                    suggestion.source = "procedural";
                    break;
                case AssetCategory::Audio:
                case AssetCategory::Music:
                    suggestion.source = "marketplace:itch.io";
                    break;
                default:
                    suggestion.source = "procedural";
                    break;
            }
            
            suggestions.push_back(suggestion);
        }
    }
    
    // Sort by priority (highest first)
    std::sort(suggestions.begin(), suggestions.end(),
        [](const AssetSuggestion& a, const AssetSuggestion& b) {
            return a.priority > b.priority;
        });
    
    return suggestions;
}

std::vector<AssetSuggestion> ProjectContext::GetPrioritySuggestions(uint32_t maxCount) const {
    auto suggestions = GenerateSuggestions();
    if (suggestions.size() > maxCount) {
        suggestions.resize(maxCount);
    }
    return suggestions;
}

// --- Serialization ---

void ProjectContext::Clear() {
    m_gameType = GameType::Unknown;
    m_projectName.clear();
    m_projectRoot.clear();
    m_inventory.categoryCounts.clear();
    m_inventory.assetIds.clear();
    m_inventory.totalAssetCount = 0;
    m_inventory.totalSizeBytes = 0;
    m_requirements.clear();
    m_assetCategories.clear();
}

std::string ProjectContext::ToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"gameType\": \"" << GameTypeToString(m_gameType) << "\",\n";
    oss << "  \"projectName\": \"" << m_projectName << "\",\n";
    oss << "  \"projectRoot\": \"" << m_projectRoot << "\",\n";
    oss << "  \"assetCount\": " << m_inventory.totalAssetCount << ",\n";
    oss << "  \"requirementsCount\": " << m_requirements.size() << "\n";
    oss << "}";
    return oss.str();
}

bool ProjectContext::FromJson(const std::string& /*json*/) {
    // Simplified implementation - would need proper JSON parsing
    return false;
}

// --- Static Helpers ---

const char* ProjectContext::GameTypeToString(GameType type) {
    switch (type) {
        case GameType::Unknown: return "unknown";
        case GameType::RPG: return "rpg";
        case GameType::FPS: return "fps";
        case GameType::TPS: return "tps";
        case GameType::Strategy: return "strategy";
        case GameType::Platformer: return "platformer";
        case GameType::Puzzle: return "puzzle";
        case GameType::Simulation: return "simulation";
        case GameType::Racing: return "racing";
        case GameType::Adventure: return "adventure";
        case GameType::Survival: return "survival";
        case GameType::Custom: return "custom";
        default: return "unknown";
    }
}

GameType ProjectContext::StringToGameType(const std::string& str) {
    if (str == "rpg") return GameType::RPG;
    if (str == "fps") return GameType::FPS;
    if (str == "tps") return GameType::TPS;
    if (str == "strategy") return GameType::Strategy;
    if (str == "platformer") return GameType::Platformer;
    if (str == "puzzle") return GameType::Puzzle;
    if (str == "simulation") return GameType::Simulation;
    if (str == "racing") return GameType::Racing;
    if (str == "adventure") return GameType::Adventure;
    if (str == "survival") return GameType::Survival;
    if (str == "custom") return GameType::Custom;
    return GameType::Unknown;
}

const char* ProjectContext::CategoryToString(AssetCategory category) {
    switch (category) {
        case AssetCategory::Unknown: return "unknown";
        case AssetCategory::Mesh3D: return "mesh3d";
        case AssetCategory::Mesh2D: return "mesh2d";
        case AssetCategory::Texture: return "texture";
        case AssetCategory::Terrain: return "terrain";
        case AssetCategory::Character: return "character";
        case AssetCategory::NPC: return "npc";
        case AssetCategory::Environment: return "environment";
        case AssetCategory::Prop: return "prop";
        case AssetCategory::VFX: return "vfx";
        case AssetCategory::Audio: return "audio";
        case AssetCategory::Music: return "music";
        case AssetCategory::UI: return "ui";
        case AssetCategory::Animation: return "animation";
        case AssetCategory::Script: return "script";
        case AssetCategory::Prefab: return "prefab";
        case AssetCategory::World: return "world";
        default: return "unknown";
    }
}

AssetCategory ProjectContext::StringToCategory(const std::string& str) {
    if (str == "mesh3d") return AssetCategory::Mesh3D;
    if (str == "mesh2d") return AssetCategory::Mesh2D;
    if (str == "texture") return AssetCategory::Texture;
    if (str == "terrain") return AssetCategory::Terrain;
    if (str == "character") return AssetCategory::Character;
    if (str == "npc") return AssetCategory::NPC;
    if (str == "environment") return AssetCategory::Environment;
    if (str == "prop") return AssetCategory::Prop;
    if (str == "vfx") return AssetCategory::VFX;
    if (str == "audio") return AssetCategory::Audio;
    if (str == "music") return AssetCategory::Music;
    if (str == "ui") return AssetCategory::UI;
    if (str == "animation") return AssetCategory::Animation;
    if (str == "script") return AssetCategory::Script;
    if (str == "prefab") return AssetCategory::Prefab;
    if (str == "world") return AssetCategory::World;
    return AssetCategory::Unknown;
}

} // namespace atlas::ai
