#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace atlas::mod {

class AssetRegistry {
public:
    void RegisterGraph(const std::string& path);
    void RegisterArchetype(const std::string& path);

    const std::vector<std::string>& Graphs() const { return graphs; }
    const std::vector<std::string>& Archetypes() const { return archetypes; }

    size_t GraphCount() const { return graphs.size(); }
    size_t ArchetypeCount() const { return archetypes.size(); }

private:
    std::vector<std::string> graphs;
    std::vector<std::string> archetypes;
};

} // namespace atlas::mod
