#include "FactionRouter.h"

namespace atlas::ai {

using namespace interaction;

Utterance FactionRouter::Route(
    const Intent& intent,
    const Utterance&)
{
    if (intent.name == "RequestResource") {
        float adjustedTrust = diplomacy.trust - personality.paranoia * 0.1f;

        if (adjustedTrust > 0.7f)
            return { "We will send the resources." };

        if (diplomacy.hostility > 0.5f)
            return { "Request denied. Do not ask again." };

        return { "What do we receive in return?" };
    }

    if (intent.name == "Threaten") {
        float hostilityDelta =
            personality.aggression * 0.3f +
            personality.paranoia * 0.2f;
        diplomacy.hostility += hostilityDelta;
        return { "This will not be forgotten." };
    }

    return { "We have no response." };
}

} // namespace atlas::ai
