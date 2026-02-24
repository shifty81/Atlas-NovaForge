#pragma once
#include <string>

namespace atlas::world {

class ExplainableNode {
public:
    virtual ~ExplainableNode() = default;
    virtual std::string Explain() const = 0;
};

} // namespace atlas::world
