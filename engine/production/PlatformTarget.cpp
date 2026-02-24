#include "PlatformTarget.h"

namespace atlas::production {

PlatformConfig PlatformTarget::DefaultFor(Platform platform) {
    PlatformConfig config;
    config.platform = platform;
    switch (platform) {
        case Platform::Windows:
            config.architecture = "x86_64";
            config.outputExtension = ".exe";
            config.toolchain = "msvc";
            config.defines = {"ATLAS_PLATFORM_WINDOWS", "WIN32"};
            break;
        case Platform::Linux:
            config.architecture = "x86_64";
            config.outputExtension = "";
            config.toolchain = "gcc";
            config.defines = {"ATLAS_PLATFORM_LINUX"};
            break;
        case Platform::macOS:
            config.architecture = "arm64";
            config.outputExtension = ".app";
            config.toolchain = "clang";
            config.defines = {"ATLAS_PLATFORM_MACOS"};
            break;
        case Platform::Web:
            config.architecture = "wasm32";
            config.outputExtension = ".html";
            config.toolchain = "emscripten";
            config.defines = {"ATLAS_PLATFORM_WEB", "EMSCRIPTEN"};
            config.excludedModules = {"voice", "server"};
            break;
    }
    return config;
}

const char* PlatformTarget::PlatformName(Platform platform) {
    switch (platform) {
        case Platform::Windows: return "Windows";
        case Platform::Linux: return "Linux";
        case Platform::macOS: return "macOS";
        case Platform::Web: return "Web";
    }
    return "Unknown";
}

Platform PlatformTarget::HostPlatform() {
#if defined(_WIN32)
    return Platform::Windows;
#elif defined(__APPLE__)
    return Platform::macOS;
#elif defined(__EMSCRIPTEN__)
    return Platform::Web;
#else
    return Platform::Linux;
#endif
}

void PlatformTarget::SetTarget(Platform platform) {
    m_config = DefaultFor(platform);
}

void PlatformTarget::SetTargetConfig(const PlatformConfig& config) {
    m_config = config;
}

Platform PlatformTarget::ActivePlatform() const {
    return m_config.platform;
}

const PlatformConfig& PlatformTarget::ActiveConfig() const {
    return m_config;
}

bool PlatformTarget::IsModuleExcluded(const std::string& moduleName) const {
    for (const auto& excluded : m_config.excludedModules) {
        if (excluded == moduleName) return true;
    }
    return false;
}

std::vector<Platform> PlatformTarget::SupportedPlatforms() {
    return { Platform::Windows, Platform::Linux, Platform::macOS, Platform::Web };
}

}
