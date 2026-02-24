#pragma once
// ============================================================
// Atlas Asset Validator — Hash Immutability & Dependency Tracking
// ============================================================
//
// Validates .atlasb files against their stored hashes, manages
// version migration rules, and tracks asset dependency graphs
// with circular dependency detection.
//
// See: docs/IMPLEMENTATION_PATH.md (Priority 5.2)

#include "AssetFormat.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>

namespace atlas::asset {

/// Result of validating a single asset file.
struct ValidationResult {
    bool valid = false;
    std::string assetId;
    uint64_t expectedHash = 0;
    uint64_t actualHash = 0;
    bool hashMatch = false;
    bool formatValid = false;
    std::string errorMessage;
};

/// Describes a single version migration step.
struct MigrationRule {
    uint16_t fromVersion;
    uint16_t toVersion;
    std::string description;
};

/// A directed edge in the asset dependency graph.
struct AssetDependency {
    std::string assetId;
    std::string dependsOn;
};

/// Validates assets, manages migrations, and tracks dependencies.
class AssetValidator {
public:
    // --- Hash immutability enforcement ---

    ValidationResult Validate(const std::string& filePath);
    ValidationResult ValidateHeader(const std::string& filePath);
    static uint64_t ComputeFileHash(const std::string& filePath);

    /// Lock an asset so it cannot be modified. Returns true on success.
    bool LockAsset(const std::string& assetId);

    /// Check whether an asset is locked (immutable).
    bool IsAssetLocked(const std::string& assetId) const;

    /// Returns the set of all locked asset IDs.
    std::vector<std::string> LockedAssets() const;

    // --- Schema version locking ---

    /// Lock the asset schema at the current version, preventing further changes.
    bool LockSchema(uint16_t version);

    /// Check whether the asset schema is locked.
    bool IsSchemaLocked() const;

    /// Returns the locked schema version, or 0 if not locked.
    uint16_t LockedSchemaVersion() const;

    // --- Version migration ---

    void RegisterMigration(uint16_t fromVersion, uint16_t toVersion,
                           const std::string& desc);
    bool CanMigrate(uint16_t fromVersion, uint16_t toVersion) const;
    std::vector<MigrationRule> MigrationPath(uint16_t from, uint16_t to) const;

    // --- Dependency tracking ---

    void AddDependency(const std::string& assetId, const std::string& dependsOn);
    std::vector<std::string> GetDependencies(const std::string& assetId) const;
    std::vector<std::string> GetDependents(const std::string& assetId) const;
    bool HasCircularDependency(const std::string& assetId) const;
    std::vector<AssetDependency> AllDependencies() const;

    size_t MigrationCount() const;
    size_t DependencyCount() const;

private:
    std::vector<MigrationRule> m_migrations;
    std::vector<AssetDependency> m_dependencies;
    std::unordered_set<std::string> m_lockedAssets;
    bool m_schemaLocked = false;
    uint16_t m_lockedSchemaVersion = 0;
};

}  // namespace atlas::asset
