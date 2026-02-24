#include "ProjectManager.h"
#include "../core/Logger.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace atlas::project {

ProjectManager& ProjectManager::Get() {
    static ProjectManager instance;
    return instance;
}

// Minimal JSON value parser for project files
// Supports strings, integers, and booleans in a flat/nested object structure
namespace {

std::string Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

std::string ExtractStringValue(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";

    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return "";

    size_t end = json.find('"', pos + 1);
    if (end == std::string::npos) return "";

    return json.substr(pos + 1, end - pos - 1);
}

int ExtractIntValue(const std::string& json, const std::string& key, int defaultValue) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return defaultValue;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return defaultValue;

    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    if (pos >= json.size()) return defaultValue;

    std::string numStr;
    while (pos < json.size() && (json[pos] >= '0' && json[pos] <= '9')) {
        numStr += json[pos++];
    }

    if (numStr.empty()) return defaultValue;
    int value = std::stoi(numStr);
    if (value < 0) return defaultValue;
    return value;
}

bool ExtractBoolValue(const std::string& json, const std::string& key, bool defaultValue) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return defaultValue;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return defaultValue;

    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;

    if (pos + 3 < json.size() && json.substr(pos, 4) == "true") return true;
    if (pos + 4 < json.size() && json.substr(pos, 5) == "false") return false;

    return defaultValue;
}

} // anonymous namespace

bool ProjectManager::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::Error("Failed to open project file: " + path);
        m_loaded = false;
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();

    // Parse required fields
    m_descriptor.schema = ExtractStringValue(json, "schema");
    m_descriptor.name = ExtractStringValue(json, "name");
    m_descriptor.version = ExtractStringValue(json, "version");
    m_descriptor.description = ExtractStringValue(json, "description");

    // Parse modules
    m_descriptor.modules.worldGraph = ExtractStringValue(json, "worldGraph");
    m_descriptor.modules.strategyGraphs = ExtractStringValue(json, "strategyGraphs");
    m_descriptor.modules.ai = ExtractBoolValue(json, "ai", false);
    m_descriptor.modules.content = ExtractStringValue(json, "content");

    // Parse runtime
    m_descriptor.runtime.entryWorld = ExtractStringValue(json, "entryWorld");
    m_descriptor.runtime.tickRate = static_cast<uint32_t>(
        ExtractIntValue(json, "tickRate", 30));
    m_descriptor.runtime.maxPlayers = static_cast<uint32_t>(
        ExtractIntValue(json, "maxPlayers", 20));

    // Parse assets
    std::string assetRoot = ExtractStringValue(json, "root");
    if (!assetRoot.empty()) {
        m_descriptor.assets.root = assetRoot;
    }

    // Store project root directory
    std::filesystem::path p(path);
    m_projectRoot = p.parent_path().string();

    // Validate
    if (!Validate()) {
        Logger::Error("Project validation failed: " + path);
        m_loaded = false;
        return false;
    }

    m_loaded = true;
    Logger::Info("Project loaded: " + m_descriptor.name + " v" + m_descriptor.version);
    return true;
}

void ProjectManager::Unload() {
    m_descriptor = ProjectDescriptor{};
    m_loaded = false;
    m_projectRoot.clear();
}

bool ProjectManager::IsLoaded() const {
    return m_loaded;
}

const ProjectDescriptor& ProjectManager::Descriptor() const {
    return m_descriptor;
}

const std::string& ProjectManager::ProjectRoot() const {
    return m_projectRoot;
}

bool ProjectManager::Validate() const {
    if (!ValidateSchema(m_descriptor.schema)) {
        Logger::Error("Invalid schema: " + m_descriptor.schema);
        return false;
    }

    if (m_descriptor.name.empty()) {
        Logger::Error("Project name is required");
        return false;
    }

    if (m_descriptor.version.empty()) {
        Logger::Error("Project version is required");
        return false;
    }

    // Validate version format (X.Y.Z)
    int dots = 0;
    for (char c : m_descriptor.version) {
        if (c == '.') dots++;
        else if (c < '0' || c > '9') {
            Logger::Error("Invalid version format: " + m_descriptor.version);
            return false;
        }
    }
    if (dots != 2) {
        Logger::Error("Version must be in X.Y.Z format: " + m_descriptor.version);
        return false;
    }

    if (m_descriptor.runtime.tickRate < 1 || m_descriptor.runtime.tickRate > 120) {
        Logger::Error("Tick rate must be between 1 and 120");
        return false;
    }

    return true;
}

bool ProjectManager::ValidateSchema(const std::string& schemaField) {
    return schemaField == "atlas.project.v1";
}

}
