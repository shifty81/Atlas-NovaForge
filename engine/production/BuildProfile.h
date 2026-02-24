#pragma once
#include <string>
#include <unordered_map>

namespace atlas::production {

enum class ProfileType {
    Debug,
    Development,
    Release,
};

struct BuildProfileConfig {
    ProfileType type = ProfileType::Debug;
    bool optimizationsEnabled = false;
    bool debugSymbols = true;
    bool assertionsEnabled = true;
    bool stripEditorData = false;
    bool enableProfiling = true;
    bool enableLogging = true;
    std::string compilerFlags;
    std::string linkerFlags;
};

class BuildProfile {
public:
    // Get default profile for a given type
    static BuildProfileConfig DefaultFor(ProfileType type);

    // Get profile name string
    static const char* ProfileName(ProfileType type);

    // Active profile management
    void SetActive(ProfileType type);
    void SetActiveConfig(const BuildProfileConfig& config);
    ProfileType ActiveType() const;
    const BuildProfileConfig& ActiveConfig() const;

    // Custom overrides
    void SetOverride(const std::string& key, const std::string& value);
    std::string GetOverride(const std::string& key) const;
    bool HasOverride(const std::string& key) const;
    void ClearOverrides();
    size_t OverrideCount() const;

private:
    BuildProfileConfig m_active;
    std::unordered_map<std::string, std::string> m_overrides;
};

}
