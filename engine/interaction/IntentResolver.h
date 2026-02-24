#pragma once
#include "Utterance.h"
#include "Intent.h"
#include <optional>

namespace atlas::interaction {

class IntentResolver {
public:
    virtual ~IntentResolver() = default;
    virtual std::optional<Intent> Resolve(const Utterance&) = 0;
};

} // namespace atlas::interaction
