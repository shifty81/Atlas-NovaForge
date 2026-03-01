#pragma once
#include <string>
#include <vector>
#include <memory>

namespace atlas::plugin {

struct PluginDescriptor {
    std::string name;
    std::string version;
    std::string atlasVersion;
    std::string type; // "graph-extension", "editor-panel", "asset-importer"
    bool deterministic = true;
};

enum class PluginLoadResult { Success, NotFound, InvalidDescriptor, IncompatibleVersion, NonDeterministic };

class PluginValidator {
public:
    static PluginLoadResult Validate(const PluginDescriptor& descriptor, const std::string& engineVersion = "0.1.0");
    static bool IsVersionCompatible(const std::string& pluginVersion, const std::string& engineVersion);
};

class PluginRegistry {
public:
    bool RegisterPlugin(const PluginDescriptor& descriptor);
    bool UnregisterPlugin(const std::string& name);
    const PluginDescriptor* FindPlugin(const std::string& name) const;
    size_t PluginCount() const;
    std::vector<PluginDescriptor> GetAll() const;
    std::vector<PluginDescriptor> GetByType(const std::string& type) const;
private:
    std::vector<PluginDescriptor> m_plugins;
};

} // namespace atlas::plugin
