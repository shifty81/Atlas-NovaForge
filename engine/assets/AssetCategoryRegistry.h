#pragma once
// ============================================================
// Atlas Asset Category Registry — Type & System Classification
// ============================================================
//
// Categorizes assets by their content type (visual, audio, data)
// and by the engine system that consumes them (gameplay, UI, world,
// rendering, etc.). This enables filtered browsing in the editor,
// validation that assets are used in appropriate contexts, and
// structured import from external marketplaces.
//
// See: docs/03_ASSET_SYSTEM.md

#include "AssetFormat.h"
#include <cstdint>
#include <string>
#include <vector>

namespace atlas::asset {

/// High-level content category for assets.
enum class ContentCategory : uint8_t {
    Visual,     ///< Meshes, textures, materials, sprites.
    Audio,      ///< Sound effects, music, voice.
    Data,       ///< Scripts, configs, gameplay definitions.
    Font,       ///< TrueType, OpenType fonts.
    Composite,  ///< Prefabs, scenes, tilesets.
};

/// Engine system that consumes an asset.
enum class SystemUsage : uint8_t {
    Gameplay,   ///< Combat, economy, progression.
    UI,         ///< HUD, menus, widget art.
    World,      ///< Terrain, environment, skybox.
    Rendering,  ///< Shaders, post-processing, VFX.
    AI,         ///< Behavior trees, decision data.
    Animation,  ///< Skeletal anims, blend trees.
    Physics,    ///< Collision shapes, ragdoll data.
    General,    ///< Not tied to a specific system.
};

/// Full classification entry for an asset type.
struct AssetCategoryEntry {
    AssetType type = AssetType::Graph;
    ContentCategory content = ContentCategory::Data;
    SystemUsage system = SystemUsage::General;
    std::string label;        ///< Human-readable name.
    std::string description;  ///< Short description.
};

/// Registry that maps asset types to their categories and provides
/// filtered queries for editor browsing and validation.
class AssetCategoryRegistry {
public:
    /// Register a category entry. Replaces any existing entry for the same AssetType.
    void Register(AssetCategoryEntry entry);

    /// Remove a category entry by AssetType.
    bool Unregister(AssetType type);

    /// Look up a category entry by AssetType. Returns nullptr if not found.
    const AssetCategoryEntry* Find(AssetType type) const;

    /// Get all registered entries.
    const std::vector<AssetCategoryEntry>& Entries() const;

    /// Number of registered entries.
    size_t EntryCount() const;

    /// Get all entries matching a content category.
    std::vector<AssetCategoryEntry> ByContent(ContentCategory category) const;

    /// Get all entries matching a system usage.
    std::vector<AssetCategoryEntry> BySystem(SystemUsage system) const;

    /// Get the content category for an AssetType (Data if not registered).
    ContentCategory ContentFor(AssetType type) const;

    /// Get the system usage for an AssetType (General if not registered).
    SystemUsage SystemFor(AssetType type) const;

    /// Populate the registry with default mappings for all built-in AssetTypes.
    void RegisterDefaults();

    /// Remove all entries.
    void Clear();

private:
    std::vector<AssetCategoryEntry> m_entries;
};

}  // namespace atlas::asset
