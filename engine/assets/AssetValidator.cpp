#include "AssetValidator.h"
#include "../sim/StateHasher.h"
#include <filesystem>
#include <fstream>
#include <unordered_set>

namespace atlas::asset {

// ---------------------------------------------------------------------------
// Hash computation
// ---------------------------------------------------------------------------

uint64_t AssetValidator::ComputeFileHash(const std::string& filePath) {
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open()) return 0;

    // Skip the header to hash only the data portion
    AssetHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in.good()) return 0;

    std::vector<uint8_t> data(hdr.size);
    in.read(reinterpret_cast<char*>(data.data()),
            static_cast<std::streamsize>(hdr.size));
    if (!in.good()) return 0;

    return sim::StateHasher::HashCombine(0, data.data(), data.size());
}

// ---------------------------------------------------------------------------
// Header validation
// ---------------------------------------------------------------------------

ValidationResult AssetValidator::ValidateHeader(const std::string& filePath) {
    ValidationResult result;
    result.assetId = std::filesystem::path(filePath).stem().string();

    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open()) {
        result.errorMessage = "Cannot open file: " + filePath;
        return result;
    }

    AssetHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in.good()) {
        result.errorMessage = "Failed to read header: " + filePath;
        return result;
    }

    if (hdr.magic != ASSET_MAGIC) {
        result.errorMessage = "Invalid magic number";
        return result;
    }

    result.formatValid = true;
    result.expectedHash = hdr.hash;
    result.valid = true;
    return result;
}

// ---------------------------------------------------------------------------
// Full validation (header + hash check)
// ---------------------------------------------------------------------------

ValidationResult AssetValidator::Validate(const std::string& filePath) {
    ValidationResult result = ValidateHeader(filePath);
    if (!result.formatValid) return result;

    result.actualHash = ComputeFileHash(filePath);
    result.hashMatch = (result.expectedHash == result.actualHash);
    result.valid = result.formatValid && result.hashMatch;

    if (!result.hashMatch) {
        result.errorMessage = "Hash mismatch: expected "
            + std::to_string(result.expectedHash)
            + " got " + std::to_string(result.actualHash);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Version migration
// ---------------------------------------------------------------------------

void AssetValidator::RegisterMigration(uint16_t fromVersion, uint16_t toVersion,
                                        const std::string& desc) {
    MigrationRule rule;
    rule.fromVersion = fromVersion;
    rule.toVersion = toVersion;
    rule.description = desc;
    m_migrations.push_back(std::move(rule));
}

bool AssetValidator::CanMigrate(uint16_t fromVersion, uint16_t toVersion) const {
    return !MigrationPath(fromVersion, toVersion).empty();
}

std::vector<MigrationRule> AssetValidator::MigrationPath(uint16_t from,
                                                          uint16_t to) const {
    // BFS to find shortest migration chain
    if (from == to) return {};

    struct Step {
        uint16_t version;
        std::vector<MigrationRule> path;
    };

    std::vector<Step> queue;
    std::unordered_set<uint16_t> visited;

    Step start;
    start.version = from;
    queue.push_back(std::move(start));
    visited.insert(from);

    size_t head = 0;
    while (head < queue.size()) {
        Step current = queue[head++];

        for (const auto& rule : m_migrations) {
            if (rule.fromVersion != current.version) continue;
            if (visited.count(rule.toVersion)) continue;

            Step next;
            next.version = rule.toVersion;
            next.path = current.path;
            next.path.push_back(rule);

            if (next.version == to) return next.path;

            visited.insert(next.version);
            queue.push_back(std::move(next));
        }
    }

    return {};  // No path found
}

// ---------------------------------------------------------------------------
// Dependency tracking
// ---------------------------------------------------------------------------

void AssetValidator::AddDependency(const std::string& assetId,
                                    const std::string& dependsOn) {
    AssetDependency dep;
    dep.assetId = assetId;
    dep.dependsOn = dependsOn;
    m_dependencies.push_back(std::move(dep));
}

std::vector<std::string> AssetValidator::GetDependencies(
        const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId) {
            result.push_back(dep.dependsOn);
        }
    }
    return result;
}

std::vector<std::string> AssetValidator::GetDependents(
        const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.dependsOn == assetId) {
            result.push_back(dep.assetId);
        }
    }
    return result;
}

bool AssetValidator::HasCircularDependency(const std::string& assetId) const {
    // DFS reachability check: a cycle exists if any transitive
    // dependency of assetId leads back to assetId itself.
    std::unordered_set<std::string> visited;
    std::vector<std::string> stack;

    // Seed with direct dependencies of the starting asset
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId) {
            stack.push_back(dep.dependsOn);
        }
    }

    while (!stack.empty()) {
        std::string current = stack.back();
        stack.pop_back();

        if (current == assetId) return true;

        if (visited.count(current)) continue;
        visited.insert(current);

        for (const auto& dep : m_dependencies) {
            if (dep.assetId == current && !visited.count(dep.dependsOn)) {
                stack.push_back(dep.dependsOn);
            }
        }
    }

    return false;
}

std::vector<AssetDependency> AssetValidator::AllDependencies() const {
    return m_dependencies;
}

size_t AssetValidator::MigrationCount() const {
    return m_migrations.size();
}

size_t AssetValidator::DependencyCount() const {
    return m_dependencies.size();
}

// ---------------------------------------------------------------------------
// Asset immutability enforcement
// ---------------------------------------------------------------------------

bool AssetValidator::LockAsset(const std::string& assetId) {
    if (m_lockedAssets.count(assetId)) return false;
    m_lockedAssets.insert(assetId);
    return true;
}

bool AssetValidator::IsAssetLocked(const std::string& assetId) const {
    return m_lockedAssets.count(assetId) > 0;
}

std::vector<std::string> AssetValidator::LockedAssets() const {
    return std::vector<std::string>(m_lockedAssets.begin(), m_lockedAssets.end());
}

// ---------------------------------------------------------------------------
// Schema version locking
// ---------------------------------------------------------------------------

bool AssetValidator::LockSchema(uint16_t version) {
    if (m_schemaLocked) return false;
    m_schemaLocked = true;
    m_lockedSchemaVersion = version;
    return true;
}

bool AssetValidator::IsSchemaLocked() const {
    return m_schemaLocked;
}

uint16_t AssetValidator::LockedSchemaVersion() const {
    return m_lockedSchemaVersion;
}

}  // namespace atlas::asset
