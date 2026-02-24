#pragma once
#include <string>
#include <cstdint>

namespace atlas::project {

struct ModulesConfig {
    std::string worldGraph;
    std::string strategyGraphs;
    bool ai = false;
    std::string content;
};

struct RuntimeConfig {
    std::string entryWorld;
    uint32_t tickRate = 30;
    uint32_t maxPlayers = 20;
};

struct AssetsConfig {
    std::string root = "assets";
};

struct ProjectDescriptor {
    std::string schema;
    std::string name;
    std::string version;
    std::string description;
    ModulesConfig modules;
    RuntimeConfig runtime;
    AssetsConfig assets;
};

class ProjectManager {
public:
    static ProjectManager& Get();

    bool Load(const std::string& path);
    void Unload();

    bool IsLoaded() const;
    const ProjectDescriptor& Descriptor() const;
    const std::string& ProjectRoot() const;

    bool Validate() const;

    static bool ValidateSchema(const std::string& schemaField);

private:
    ProjectDescriptor m_descriptor;
    bool m_loaded = false;
    std::string m_projectRoot;
};

}
