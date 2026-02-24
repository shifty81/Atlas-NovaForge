#pragma once
#include <string>
#include <cstdint>

namespace atlas::interaction {

using EntityID = uint64_t;

enum class InteractionMode : uint8_t {
    Command,
    Conversation,
    Debug,
    Narrative
};

struct InteractionContext {
    InteractionMode mode;
    bool authoritative;   // server/editor true
};

struct Utterance {
    std::string text;
    EntityID speaker = 0;
    EntityID listener = 0;   // optional
    InteractionContext ctx;
};

} // namespace atlas::interaction
