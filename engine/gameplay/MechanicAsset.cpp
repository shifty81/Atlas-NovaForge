#include "MechanicAsset.h"
#include <algorithm>

namespace atlas::gameplay {

void MechanicRegistry::Init() {
    m_mechanics.clear();
    m_nextId = 1;
}

uint32_t MechanicRegistry::Register(const std::string& name, MechanicType type, const std::string& graphRef) {
    uint32_t id = m_nextId++;
    MechanicAsset asset;
    asset.id = id;
    asset.name = name;
    asset.type = type;
    asset.graphRef = graphRef;
    m_mechanics[id] = asset;
    return id;
}

void MechanicRegistry::Unregister(uint32_t id) {
    m_mechanics.erase(id);
}

const MechanicAsset* MechanicRegistry::Get(uint32_t id) const {
    auto it = m_mechanics.find(id);
    if (it != m_mechanics.end()) {
        return &it->second;
    }
    return nullptr;
}

const MechanicAsset* MechanicRegistry::FindByName(const std::string& name) const {
    for (auto& [id, asset] : m_mechanics) {
        if (asset.name == name) {
            return &asset;
        }
    }
    return nullptr;
}

size_t MechanicRegistry::Count() const {
    return m_mechanics.size();
}

void MechanicRegistry::AddParam(uint32_t id, const std::string& name, const std::string& type, float defaultValue) {
    auto it = m_mechanics.find(id);
    if (it != m_mechanics.end()) {
        MechanicParam param;
        param.name = name;
        param.type = type;
        param.defaultValue = defaultValue;
        it->second.params.push_back(param);
    }
}

std::vector<const MechanicAsset*> MechanicRegistry::GetByType(MechanicType type) const {
    std::vector<const MechanicAsset*> result;
    for (auto& [id, asset] : m_mechanics) {
        if (asset.type == type) {
            result.push_back(&asset);
        }
    }
    return result;
}

}
