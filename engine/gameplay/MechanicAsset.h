#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace atlas::gameplay {

enum class MechanicType : uint8_t {
    Action,
    State,
    Rule,
    Trigger,
    Modifier
};

struct MechanicParam {
    std::string name;
    std::string type;
    float defaultValue = 0.0f;
};

struct MechanicAsset {
    uint32_t id = 0;
    std::string name;
    MechanicType type = MechanicType::Action;
    std::string graphRef;
    std::vector<MechanicParam> params;
};

class MechanicRegistry {
public:
    void Init();

    uint32_t Register(const std::string& name, MechanicType type, const std::string& graphRef = "");
    void Unregister(uint32_t id);
    const MechanicAsset* Get(uint32_t id) const;
    const MechanicAsset* FindByName(const std::string& name) const;
    size_t Count() const;

    void AddParam(uint32_t id, const std::string& name, const std::string& type, float defaultValue = 0.0f);

    std::vector<const MechanicAsset*> GetByType(MechanicType type) const;

private:
    std::unordered_map<uint32_t, MechanicAsset> m_mechanics;
    uint32_t m_nextId = 1;
};

}
