#pragma once
#include "AssetGraph.h"
#include <vector>
#include <memory>

namespace atlas::asset {

class AssetGraphExecutor {
public:
    void AddNode(std::unique_ptr<AssetNode> node);
    void Execute(const AssetContext& ctx);
    size_t NodeCount() const;

private:
    std::vector<std::unique_ptr<AssetNode>> nodes;
};

} // namespace atlas::asset
