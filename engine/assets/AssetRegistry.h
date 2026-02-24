#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <set>
#include <stack>

namespace atlas::asset {

struct AssetEntry {
    std::string id;
    std::string path;
    uint64_t version = 1;
};

struct AssetDependency {
    std::string assetId;
    std::string dependsOn;
};

class AssetRegistry {
public:
    using ReloadCallback = std::function<void(const AssetEntry&)>;

    void Scan(const std::string& root);
    const AssetEntry* Get(const std::string& id) const;
    std::vector<AssetEntry> GetAll() const;

    void SetReloadCallback(ReloadCallback cb);
    void PollHotReload();

    size_t Count() const;

    void AddDependency(const std::string& assetId, const std::string& dependsOn);
    void RemoveDependency(const std::string& assetId, const std::string& dependsOn);
    std::vector<std::string> GetDependencies(const std::string& assetId) const;
    std::vector<std::string> GetDependents(const std::string& assetId) const;
    bool HasCircularDependency(const std::string& assetId) const;
    std::vector<std::string> ResolveBuildOrder() const;
    void ClearDependencies();

private:
    std::unordered_map<std::string, AssetEntry> m_assets;
    std::unordered_map<std::string, std::filesystem::file_time_type> m_timestamps;
    ReloadCallback m_onReload;
    std::vector<AssetDependency> m_dependencies;
};

}
