#include "ServerRules.h"
#include "../core/Logger.h"
#include <sstream>

namespace atlas::rules {

ServerRules& ServerRules::Get() {
    static ServerRules instance;
    return instance;
}

void ServerRules::RegisterRule(const RuleDescriptor& desc) {
    m_rules[desc.name] = desc;
}

bool ServerRules::HasRule(const std::string& name) const {
    return m_rules.count(name) > 0;
}

float ServerRules::GetValue(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return 0.0f;
    return it->second.baseValue * it->second.multiplier;
}

float ServerRules::GetMultiplier(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return 1.0f;
    return it->second.multiplier;
}

bool ServerRules::SetMultiplier(const std::string& name, float multiplier) {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return false;

    auto& rule = it->second;

    // Clamp to allowed range
    if (multiplier < rule.minMultiplier) multiplier = rule.minMultiplier;
    if (multiplier > rule.maxMultiplier) multiplier = rule.maxMultiplier;

    rule.multiplier = multiplier;
    return true;
}

const RuleDescriptor* ServerRules::GetDescriptor(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it != m_rules.end()) return &it->second;
    return nullptr;
}

size_t ServerRules::RuleCount() const {
    return m_rules.size();
}

void ServerRules::Clear() {
    m_rules.clear();
    m_hasPendingChanges = false;
    m_lastModifiedRules.clear();
}

size_t ServerRules::LoadFromConfig(const std::string& configText) {
    size_t count = 0;
    std::istringstream stream(configText);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string name = line.substr(0, eq);
        std::string valStr = line.substr(eq + 1);
        float multiplier = 1.0f;
        try { multiplier = std::stof(valStr); } catch (...) {
            Logger::Info("[ServerRules] LoadFromConfig: skipping malformed value '" + valStr + "' for rule '" + name + "'");
            continue;
        }
        RuleDescriptor desc;
        desc.name = name;
        desc.multiplier = multiplier;
        m_rules[name] = desc;
        ++count;
    }
    return count;
}

std::string ServerRules::ExportToConfig() const {
    std::string result;
    for (const auto& [name, desc] : m_rules) {
        result += name + "=" + std::to_string(desc.multiplier) + "\n";
    }
    return result;
}

size_t ServerRules::HotReloadFromConfig(const std::string& configText) {
    size_t count = 0;
    m_lastModifiedRules.clear();
    std::istringstream stream(configText);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string name = line.substr(0, eq);
        std::string valStr = line.substr(eq + 1);
        float multiplier = 1.0f;
        try { multiplier = std::stof(valStr); } catch (...) {
            Logger::Info("[ServerRules] HotReloadFromConfig: skipping malformed value '" + valStr + "' for rule '" + name + "'");
            continue;
        }

        auto it = m_rules.find(name);
        if (it == m_rules.end()) continue;
        if (!it->second.hotReloadable) continue;

        it->second.multiplier = multiplier;
        m_lastModifiedRules.push_back(name);
        ++count;
    }
    if (count > 0) m_hasPendingChanges = true;
    return count;
}

bool ServerRules::HasPendingChanges() const {
    return m_hasPendingChanges;
}

void ServerRules::AcknowledgeChanges() {
    m_hasPendingChanges = false;
    m_lastModifiedRules.clear();
}

std::vector<std::string> ServerRules::LastModifiedRules() const {
    return m_lastModifiedRules;
}

}
