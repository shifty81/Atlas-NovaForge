#pragma once
#include <string>
#include <vector>

namespace atlas::production {

enum class Platform {
    Windows,
    Linux,
    macOS,
    Web,
};

struct PlatformConfig {
    Platform platform = Platform::Linux;
    std::string architecture = "x86_64";
    std::string outputExtension;
    std::string toolchain;
    std::vector<std::string> defines;
    std::vector<std::string> excludedModules;
};

class PlatformTarget {
public:
    // Get default config for a platform
    static PlatformConfig DefaultFor(Platform platform);

    // Get platform name string
    static const char* PlatformName(Platform platform);

    // Get host platform
    static Platform HostPlatform();

    // Active target management
    void SetTarget(Platform platform);
    void SetTargetConfig(const PlatformConfig& config);
    Platform ActivePlatform() const;
    const PlatformConfig& ActiveConfig() const;

    // Check if a module is excluded for current platform
    bool IsModuleExcluded(const std::string& moduleName) const;

    // Get supported platforms
    static std::vector<Platform> SupportedPlatforms();

private:
    PlatformConfig m_config;
};

}
