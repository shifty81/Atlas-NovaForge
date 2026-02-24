#include "BuildProfile.h"

namespace atlas::production {

BuildProfileConfig BuildProfile::DefaultFor(ProfileType type) {
    BuildProfileConfig config;
    config.type = type;
    switch (type) {
        case ProfileType::Debug:
            config.optimizationsEnabled = false;
            config.debugSymbols = true;
            config.assertionsEnabled = true;
            config.stripEditorData = false;
            config.enableProfiling = true;
            config.enableLogging = true;
            config.compilerFlags = "-O0 -g";
            config.linkerFlags = "";
            break;
        case ProfileType::Development:
            config.optimizationsEnabled = true;
            config.debugSymbols = true;
            config.assertionsEnabled = true;
            config.stripEditorData = false;
            config.enableProfiling = true;
            config.enableLogging = true;
            config.compilerFlags = "-O2 -g";
            config.linkerFlags = "";
            break;
        case ProfileType::Release:
            config.optimizationsEnabled = true;
            config.debugSymbols = false;
            config.assertionsEnabled = false;
            config.stripEditorData = true;
            config.enableProfiling = false;
            config.enableLogging = false;
            config.compilerFlags = "-O3";
            config.linkerFlags = "-s";
            break;
    }
    return config;
}

const char* BuildProfile::ProfileName(ProfileType type) {
    switch (type) {
        case ProfileType::Debug: return "Debug";
        case ProfileType::Development: return "Development";
        case ProfileType::Release: return "Release";
        default: return "Unknown";
    }
}

void BuildProfile::SetActive(ProfileType type) {
    m_active = DefaultFor(type);
}

void BuildProfile::SetActiveConfig(const BuildProfileConfig& config) {
    m_active = config;
}

ProfileType BuildProfile::ActiveType() const {
    return m_active.type;
}

const BuildProfileConfig& BuildProfile::ActiveConfig() const {
    return m_active;
}

void BuildProfile::SetOverride(const std::string& key, const std::string& value) {
    m_overrides[key] = value;
}

std::string BuildProfile::GetOverride(const std::string& key) const {
    auto it = m_overrides.find(key);
    if (it != m_overrides.end()) return it->second;
    return "";
}

bool BuildProfile::HasOverride(const std::string& key) const {
    return m_overrides.find(key) != m_overrides.end();
}

void BuildProfile::ClearOverrides() {
    m_overrides.clear();
}

size_t BuildProfile::OverrideCount() const {
    return m_overrides.size();
}

}
