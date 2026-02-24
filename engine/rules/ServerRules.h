#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace atlas::rules {

struct RuleDescriptor {
    std::string name;
    float baseValue = 1.0f;
    float multiplier = 1.0f;
    float minMultiplier = 0.0f;
    float maxMultiplier = 10.0f;
    bool hotReloadable = false;
    bool affectsReplay = false;
};

class ServerRules {
public:
    static ServerRules& Get();

    void RegisterRule(const RuleDescriptor& desc);
    bool HasRule(const std::string& name) const;

    float GetValue(const std::string& name) const;
    float GetMultiplier(const std::string& name) const;

    bool SetMultiplier(const std::string& name, float multiplier);

    const RuleDescriptor* GetDescriptor(const std::string& name) const;
    size_t RuleCount() const;

    void Clear();

    /// Load rules from a config string (key=value format, one per line)
    size_t LoadFromConfig(const std::string& configText);

    /// Export current rules to config string format
    std::string ExportToConfig() const;

    /// Apply a hot-reload update. Only rules marked hotReloadable will be updated.
    /// Returns the number of rules actually updated.
    size_t HotReloadFromConfig(const std::string& configText);

    /// Returns true if any rules were modified since last call to AcknowledgeChanges()
    bool HasPendingChanges() const;

    /// Clear the pending changes flag
    void AcknowledgeChanges();

    /// Get list of rule names that were modified in the last hot-reload
    std::vector<std::string> LastModifiedRules() const;

private:
    std::unordered_map<std::string, RuleDescriptor> m_rules;
    bool m_hasPendingChanges = false;
    std::vector<std::string> m_lastModifiedRules;
};

}
