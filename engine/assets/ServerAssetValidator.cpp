#include "ServerAssetValidator.h"
#include <filesystem>

namespace atlas::asset {

void ServerAssetValidator::RegisterAsset(const std::string& assetId,
                                          uint64_t expectedHash,
                                          uint16_t expectedVersion) {
    ManifestEntry entry;
    entry.assetId = assetId;
    entry.expectedHash = expectedHash;
    entry.expectedVersion = expectedVersion;
    m_manifest[assetId] = entry;
}

size_t ServerAssetValidator::ManifestSize() const {
    return m_manifest.size();
}

bool ServerAssetValidator::HasAsset(const std::string& assetId) const {
    return m_manifest.count(assetId) > 0;
}

ServerValidationResult ServerAssetValidator::ValidateAsset(
    const std::string& assetId, const std::string& filePath) {

    ServerValidationResult result;
    result.assetId = assetId;

    auto it = m_manifest.find(assetId);
    if (it == m_manifest.end()) {
        result.accepted = false;
        result.reason = "Asset '" + assetId + "' not in server manifest";
        return result;
    }

    if (!std::filesystem::exists(filePath)) {
        result.accepted = false;
        result.reason = "File not found: " + filePath;
        return result;
    }

    uint64_t actualHash = AssetValidator::ComputeFileHash(filePath);
    if (actualHash != it->second.expectedHash) {
        result.accepted = false;
        result.reason = "Hash mismatch for '" + assetId +
            "': expected " + std::to_string(it->second.expectedHash) +
            ", got " + std::to_string(actualHash);
        return result;
    }

    result.accepted = true;
    return result;
}

ServerValidationResult ServerAssetValidator::ValidateHash(
    const std::string& assetId, uint64_t actualHash) {

    ServerValidationResult result;
    result.assetId = assetId;

    auto it = m_manifest.find(assetId);
    if (it == m_manifest.end()) {
        result.accepted = false;
        result.reason = "Asset '" + assetId + "' not in server manifest";
        return result;
    }

    if (actualHash != it->second.expectedHash) {
        result.accepted = false;
        result.reason = "Hash mismatch for '" + assetId + "'";
        return result;
    }

    result.accepted = true;
    return result;
}

uint32_t ServerAssetValidator::ValidateAll(
    const std::string& directory,
    std::vector<ServerValidationResult>& results) {

    uint32_t failCount = 0;

    for (const auto& [assetId, entry] : m_manifest) {
        std::string filePath = directory + "/" + assetId + ".atlasb";
        auto result = ValidateAsset(assetId, filePath);
        if (!result.accepted) ++failCount;
        results.push_back(std::move(result));
    }

    return failCount;
}

std::vector<ManifestEntry> ServerAssetValidator::Manifest() const {
    std::vector<ManifestEntry> entries;
    entries.reserve(m_manifest.size());
    for (const auto& [_, entry] : m_manifest) {
        entries.push_back(entry);
    }
    return entries;
}

}  // namespace atlas::asset
