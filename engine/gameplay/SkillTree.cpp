#include "SkillTree.h"

namespace atlas::gameplay {

void SkillTree::Init() {
    m_nodes.clear();
    m_nextId = 1;
}

SkillID SkillTree::AddNode(const std::string& name,
                           const std::vector<SkillID>& prerequisites,
                           const std::vector<SkillEffect>& effects) {
    SkillID id = m_nextId++;
    SkillNode node;
    node.id = id;
    node.name = name;
    node.prerequisites = prerequisites;
    node.effects = effects;
    node.unlocked = false;
    m_nodes[id] = node;
    return id;
}

bool SkillTree::Unlock(SkillID id) {
    if (!CanUnlock(id)) {
        return false;
    }
    m_nodes[id].unlocked = true;
    return true;
}

bool SkillTree::IsUnlocked(SkillID id) const {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return false;
    }
    return it->second.unlocked;
}

bool SkillTree::CanUnlock(SkillID id) const {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return false;
    }
    if (it->second.unlocked) {
        return false;
    }
    for (SkillID prereq : it->second.prerequisites) {
        if (!IsUnlocked(prereq)) {
            return false;
        }
    }
    return true;
}

std::vector<SkillEffect> SkillTree::GetEffects(SkillID id) const {
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return {};
    }
    return it->second.effects;
}

size_t SkillTree::NodeCount() const {
    return m_nodes.size();
}

const SkillNode* SkillTree::GetNode(SkillID id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        return &it->second;
    }
    return nullptr;
}

}
