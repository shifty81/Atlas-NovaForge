#pragma once
#include "IntentResolver.h"
#include <vector>
#include <regex>

namespace atlas::interaction {

struct IntentRule {
    std::string intentName;
    std::regex pattern;
};

class RuleIntentResolver final : public IntentResolver {
public:
    void AddRule(const IntentRule& rule);
    std::optional<Intent> Resolve(const Utterance&) override;

private:
    std::vector<IntentRule> rules;
};

} // namespace atlas::interaction
