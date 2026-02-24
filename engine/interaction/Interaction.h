#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <variant>

namespace atlas::interaction {

using EntityID = uint64_t;

enum class InteractionMode { Command, Conversation, Debug, Narrative };
enum class IntentSource { Voice, Text, AI, Script, UI, Console };

struct InteractionContext {
    InteractionMode mode = InteractionMode::Command;
    bool isAuthoritative = false;
};

struct Utterance {
    std::string text;
    EntityID speaker = 0;
    EntityID listener = 0;
    InteractionContext context;
};

using Variant = std::variant<int, float, std::string, bool>;

struct Intent {
    std::string name;
    float confidence = 1.0f;
    IntentSource source = IntentSource::Console;
    std::unordered_map<std::string, Variant> params;
};

// IntentResolver — resolves utterances to intents
class IntentResolver {
public:
    virtual ~IntentResolver() = default;
    virtual std::optional<Intent> Resolve(const Utterance& utterance) = 0;
};

// PatternIntentResolver — simple pattern-matching resolver
struct IntentPattern {
    std::string intentName;
    std::vector<std::string> phrases;
};

class PatternIntentResolver : public IntentResolver {
public:
    void AddPattern(const IntentPattern& pattern);
    std::optional<Intent> Resolve(const Utterance& utterance) override;
private:
    std::vector<IntentPattern> m_patterns;
};

// IntentRegistry — central dispatch for intents
class IntentRegistry {
public:
    using Handler = std::function<void(const Intent&)>;

    void Register(const std::string& intentName, Handler handler);
    bool Dispatch(const Intent& intent) const;
    bool HasHandler(const std::string& intentName) const;
    size_t HandlerCount() const;
private:
    std::unordered_map<std::string, Handler> m_handlers;
};

} // namespace atlas::interaction
