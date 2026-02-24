#include "pcg/room_graph.h"
#include "pcg/hash_utils.h"

namespace atlas {
namespace pcg {

// ── Public API ─────────────────────────────────────────────────────

std::vector<RoomNode> generateRoomsForDeck(int deckIndex,
                                           const PCGContext& ctx,
                                           int shipClass) {
    uint64_t deckSeed = deriveSeed(ctx.seed, static_cast<uint64_t>(deckIndex));
    DeterministicRNG rng(deckSeed);

    int roomCount = 2 + rng.range(0, shipClass);
    if (roomCount > 5) roomCount = 5;
    if (roomCount < 2) roomCount = 2;

    std::vector<RoomNode> rooms;
    rooms.reserve(static_cast<size_t>(roomCount));

    constexpr float SPACING = 6.0f;

    for (int i = 0; i < roomCount; ++i) {
        RoomNode node{};
        node.roomId = deckIndex * 100 + i;

        // Assign room type by function rather than cycling.
        if (i == 0 && deckIndex == 0) {
            node.type = RoomType::Cockpit;
        } else if (i == roomCount - 1) {
            node.type = RoomType::Engine;
        } else if (deckIndex > 0 && i == 0) {
            node.type = RoomType::Reactor;
        } else {
            // Remaining rooms chosen by RNG from functional set.
            int pick = rng.range(0, 2);
            switch (pick) {
                case 0: node.type = RoomType::CrewQuarters;  break;
                case 1: node.type = RoomType::WeaponControl; break;
                case 2: node.type = RoomType::Corridor;      break;
                default: node.type = RoomType::CrewQuarters;  break;
            }
        }

        // Vary dimensions by room type.
        switch (node.type) {
            case RoomType::Cockpit:
                node.dimX = rng.rangeFloat(4.0f, 6.0f);
                node.dimY = rng.rangeFloat(3.0f, 4.0f);
                node.dimZ = rng.rangeFloat(5.0f, 7.0f);
                break;
            case RoomType::Reactor:
                node.dimX = rng.rangeFloat(5.0f, 8.0f);
                node.dimY = rng.rangeFloat(4.0f, 6.0f);
                node.dimZ = rng.rangeFloat(5.0f, 8.0f);
                break;
            case RoomType::CrewQuarters:
                node.dimX = rng.rangeFloat(3.0f, 5.0f);
                node.dimY = rng.rangeFloat(3.0f, 4.0f);
                node.dimZ = rng.rangeFloat(4.0f, 6.0f);
                break;
            case RoomType::Engine:
                node.dimX = rng.rangeFloat(4.0f, 7.0f);
                node.dimY = rng.rangeFloat(3.0f, 5.0f);
                node.dimZ = rng.rangeFloat(6.0f, 10.0f);
                break;
            case RoomType::WeaponControl:
                node.dimX = rng.rangeFloat(3.0f, 5.0f);
                node.dimY = rng.rangeFloat(3.0f, 4.0f);
                node.dimZ = rng.rangeFloat(4.0f, 6.0f);
                break;
            case RoomType::Corridor:
                node.dimX = rng.rangeFloat(2.0f, 3.0f);
                node.dimY = rng.rangeFloat(3.0f, 4.0f);
                node.dimZ = rng.rangeFloat(6.0f, 10.0f);
                break;
        }

        node.posX = static_cast<float>(i) * SPACING;
        node.posY = 0.0f;
        node.posZ = 0.0f;

        rooms.push_back(node);
    }

    return rooms;
}

} // namespace pcg
} // namespace atlas
