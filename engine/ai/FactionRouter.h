#pragma once
#include "../interaction/InteractionRouter.h"
#include "DiplomacyIntent.h"
#include "Personality.h"

namespace atlas::ai {

class FactionRouter final
    : public interaction::InteractionRouter
{
public:
    interaction::Utterance Route(
        const interaction::Intent& intent,
        const interaction::Utterance& source
    ) override;

    DiplomacyState& GetDiplomacy() { return diplomacy; }
    const DiplomacyState& GetDiplomacy() const { return diplomacy; }

    Personality& GetPersonality() { return personality; }
    const Personality& GetPersonality() const { return personality; }

private:
    DiplomacyState diplomacy;
    Personality personality;
};

} // namespace atlas::ai
