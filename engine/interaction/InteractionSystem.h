#pragma once
#include "IntentResolver.h"
#include "InteractionRouter.h"
#include <memory>

namespace atlas::interaction {

class InteractionSystem {
public:
    void SetResolver(std::unique_ptr<IntentResolver> r);
    void SetRouter(std::unique_ptr<InteractionRouter> rt);

    Utterance Handle(const Utterance& input);

private:
    std::unique_ptr<IntentResolver> resolver;
    std::unique_ptr<InteractionRouter> router;
};

} // namespace atlas::interaction
