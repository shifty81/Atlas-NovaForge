#include "ModAssetRegistry.h"

namespace atlas::mod {

void AssetRegistry::RegisterGraph(const std::string& path)
{
    graphs.push_back(path);
}

void AssetRegistry::RegisterArchetype(const std::string& path)
{
    archetypes.push_back(path);
}

} // namespace atlas::mod
