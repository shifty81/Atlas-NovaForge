#pragma once
// ============================================================
// Atlas Server Asset Validator
// ============================================================
//
// Validates assets on the server side to prevent tampered or
// invalid assets from being loaded.  The server rejects any
// asset whose hash does not match the expected value recorded
// in the asset manifest.
//
// See: docs/ATLAS_LOCKDOWN_CHECKLIST.md (Server-safe asset validation)

#include "AssetValidator.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace atlas::asset {

/// Entry in the server's asset manifest.
struct ManifestEntry {
    std::string assetId;
    uint64_t expectedHash = 0;
    uint16_t expectedVersion = 0;
};

/// Result of server-side asset validation.
struct ServerValidationResult {
    bool accepted = false;
    std::string assetId;
    std::string reason;
};

/// Server-side asset gate that validates assets against a known manifest.
class ServerAssetValidator {
public:
    /// Register an expected asset in the manifest.
    void RegisterAsset(const std::string& assetId, uint64_t expectedHash,
                       uint16_t expectedVersion = 1);

    /// Returns the number of entries in the manifest.
    size_t ManifestSize() const;

    /// Check whether an asset is in the manifest.
    bool HasAsset(const std::string& assetId) const;

    /// Validate a single asset file against the manifest.
    ServerValidationResult ValidateAsset(const std::string& assetId,
                                          const std::string& filePath);

    /// Validate an asset by providing its hash directly (no file I/O).
    ServerValidationResult ValidateHash(const std::string& assetId,
                                         uint64_t actualHash);

    /// Validate all registered assets from a directory.
    /// Returns the number of assets that failed validation.
    uint32_t ValidateAll(const std::string& directory,
                         std::vector<ServerValidationResult>& results);

    /// Returns the manifest entries.
    std::vector<ManifestEntry> Manifest() const;

private:
    std::unordered_map<std::string, ManifestEntry> m_manifest;
};

}  // namespace atlas::asset
