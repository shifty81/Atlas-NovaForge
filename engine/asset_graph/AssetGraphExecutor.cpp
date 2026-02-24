#include "AssetGraphExecutor.h"

namespace atlas::asset {

void AssetGraphExecutor::AddNode(std::unique_ptr<AssetNode> node)
{
    nodes.push_back(std::move(node));
}

void AssetGraphExecutor::Execute(const AssetContext& ctx)
{
    for (auto& node : nodes) {
        node->Evaluate(ctx);
    }
}

size_t AssetGraphExecutor::NodeCount() const
{
    return nodes.size();
}

} // namespace atlas::asset
