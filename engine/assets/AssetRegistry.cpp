#include "AssetRegistry.h"
#include <deque>

namespace atlas::asset {

void AssetRegistry::Scan(const std::string& root) {
    if (!std::filesystem::exists(root)) return;

    for (const auto& p : std::filesystem::recursive_directory_iterator(root)) {
        if (p.path().extension() == ".atlas" || p.path().extension() == ".atlasb") {
            AssetEntry entry;
            entry.id = p.path().stem().string();
            entry.path = p.path().string();
            entry.version = 1;

            m_assets[entry.id] = entry;
            m_timestamps[entry.path] = std::filesystem::last_write_time(p);
        }
    }
}

const AssetEntry* AssetRegistry::Get(const std::string& id) const {
    auto it = m_assets.find(id);
    return it != m_assets.end() ? &it->second : nullptr;
}

std::vector<AssetEntry> AssetRegistry::GetAll() const {
    std::vector<AssetEntry> result;
    result.reserve(m_assets.size());
    for (const auto& [id, entry] : m_assets) {
        result.push_back(entry);
    }
    return result;
}

void AssetRegistry::SetReloadCallback(ReloadCallback cb) {
    m_onReload = std::move(cb);
}

void AssetRegistry::PollHotReload() {
    for (auto& [id, asset] : m_assets) {
        if (!std::filesystem::exists(asset.path)) continue;

        auto now = std::filesystem::last_write_time(asset.path);
        if (now != m_timestamps[asset.path]) {
            m_timestamps[asset.path] = now;
            asset.version++;
            if (m_onReload) m_onReload(asset);
        }
    }
}

size_t AssetRegistry::Count() const {
    return m_assets.size();
}

void AssetRegistry::AddDependency(const std::string& assetId, const std::string& dependsOn) {
    // Avoid duplicates
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId && dep.dependsOn == dependsOn) return;
    }
    m_dependencies.push_back({assetId, dependsOn});
}

void AssetRegistry::RemoveDependency(const std::string& assetId, const std::string& dependsOn) {
    for (auto it = m_dependencies.begin(); it != m_dependencies.end(); ++it) {
        if (it->assetId == assetId && it->dependsOn == dependsOn) {
            m_dependencies.erase(it);
            return;
        }
    }
}

std::vector<std::string> AssetRegistry::GetDependencies(const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId) {
            result.push_back(dep.dependsOn);
        }
    }
    return result;
}

std::vector<std::string> AssetRegistry::GetDependents(const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.dependsOn == assetId) {
            result.push_back(dep.assetId);
        }
    }
    return result;
}

bool AssetRegistry::HasCircularDependency(const std::string& assetId) const {
    // DFS with recursion stack to detect back edges
    std::set<std::string> visited;
    std::set<std::string> inStack;
    std::stack<std::pair<std::string, bool>> stack;
    stack.push({assetId, false});

    while (!stack.empty()) {
        auto& [current, processed] = stack.top();

        if (processed) {
            inStack.erase(current);
            stack.pop();
            continue;
        }

        processed = true;

        if (visited.count(current)) {
            stack.pop();
            continue;
        }

        visited.insert(current);
        inStack.insert(current);

        for (const auto& dep : m_dependencies) {
            if (dep.assetId == current) {
                if (inStack.count(dep.dependsOn)) return true;
                if (!visited.count(dep.dependsOn)) {
                    stack.push({dep.dependsOn, false});
                }
            }
        }
    }
    return false;
}

std::vector<std::string> AssetRegistry::ResolveBuildOrder() const {
    // Collect all nodes
    std::set<std::string> allNodes;
    for (const auto& dep : m_dependencies) {
        allNodes.insert(dep.assetId);
        allNodes.insert(dep.dependsOn);
    }

    // Build adjacency and in-degree
    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::unordered_map<std::string, int> inDegree;
    for (const auto& node : allNodes) {
        inDegree[node] = 0;
    }
    for (const auto& dep : m_dependencies) {
        adj[dep.dependsOn].push_back(dep.assetId);
        inDegree[dep.assetId]++;
    }

    // Kahn's algorithm
    std::deque<std::string> queue;
    for (const auto& [node, deg] : inDegree) {
        if (deg == 0) queue.push_back(node);
    }
    std::sort(queue.begin(), queue.end());

    std::vector<std::string> result;
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop_front();
        result.push_back(current);

        if (adj.count(current)) {
            std::vector<std::string> next;
            for (const auto& neighbor : adj[current]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    next.push_back(neighbor);
                }
            }
            std::sort(next.begin(), next.end());
            for (const auto& n : next) {
                queue.push_back(n);
            }
        }
    }
    return result;
}

void AssetRegistry::ClearDependencies() {
    m_dependencies.clear();
}

}
