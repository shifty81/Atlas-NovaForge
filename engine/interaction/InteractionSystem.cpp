#include "InteractionSystem.h"

namespace atlas::interaction {

void InteractionSystem::SetResolver(std::unique_ptr<IntentResolver> r)
{
    resolver = std::move(r);
}

void InteractionSystem::SetRouter(std::unique_ptr<InteractionRouter> rt)
{
    router = std::move(rt);
}

Utterance InteractionSystem::Handle(const Utterance& input)
{
    if (!resolver || !router)
        return { "Interaction system not initialized." };

    auto intent = resolver->Resolve(input);
    if (!intent)
        return { "I didn't understand that." };

    return router->Route(*intent, input);
}

} // namespace atlas::interaction
