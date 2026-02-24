#pragma once
#include <cstdint>
#include <unordered_map>
#include <utility>

namespace atlas::ai {

class RelationshipModel {
public:
    // Set the relationship value between two entities
    void Set(uint64_t entityA, uint64_t entityB, float value);

    // Get the relationship value (returns 0.0 if no relationship exists)
    float Get(uint64_t entityA, uint64_t entityB) const;

    // Apply a delta to the relationship value
    void Modify(uint64_t entityA, uint64_t entityB, float delta);

    // Check if a relationship exists
    bool Has(uint64_t entityA, uint64_t entityB) const;

    // Remove a relationship
    void Remove(uint64_t entityA, uint64_t entityB);

    void Clear();
    size_t Count() const;

private:
    // Key is ordered pair (min, max) so A→B == B→A
    static uint64_t MakeKey(uint64_t a, uint64_t b);
    std::unordered_map<uint64_t, float> m_relationships;
};

}
