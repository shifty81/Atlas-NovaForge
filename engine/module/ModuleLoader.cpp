#include "ModuleLoader.h"
#include "../core/Logger.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace atlas::module {

ModuleLoader::ModuleLoader() = default;

ModuleLoader::~ModuleLoader() {
    Unload();
}

ModuleLoadResult ModuleLoader::Load(const std::string& path) {
    if (m_module) {
        return ModuleLoadResult::AlreadyLoaded;
    }

#ifdef _WIN32
    HMODULE lib = LoadLibraryA(path.c_str());
    if (!lib) {
        atlas::Logger::Error("ModuleLoader: failed to load " + path);
        return ModuleLoadResult::NotFound;
    }
    auto fn = reinterpret_cast<CreateGameModuleFn>(
        GetProcAddress(lib, "CreateGameModule"));
    if (!fn) {
        FreeLibrary(lib);
        atlas::Logger::Error("ModuleLoader: CreateGameModule symbol not found in " + path);
        return ModuleLoadResult::SymbolMissing;
    }
    m_handle = static_cast<void*>(lib);
#else
    void* lib = dlopen(path.c_str(), RTLD_NOW);
    if (!lib) {
        atlas::Logger::Error(std::string("ModuleLoader: ") + dlerror());
        return ModuleLoadResult::NotFound;
    }
    auto fn = reinterpret_cast<CreateGameModuleFn>(dlsym(lib, "CreateGameModule"));
    if (!fn) {
        dlclose(lib);
        atlas::Logger::Error("ModuleLoader: CreateGameModule symbol not found in " + path);
        return ModuleLoadResult::SymbolMissing;
    }
    m_handle = lib;
#endif

    m_module.reset(fn());
    return ModuleLoadResult::Success;
}

void ModuleLoader::Unload() {
    m_module.reset();

    if (m_handle) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(m_handle));
#else
        dlclose(m_handle);
#endif
        m_handle = nullptr;
    }
}

void ModuleLoader::SetStaticModule(std::unique_ptr<IGameModule> mod) {
    Unload();
    m_module = std::move(mod);
}

IGameModule* ModuleLoader::GetModule() const {
    return m_module.get();
}

bool ModuleLoader::IsLoaded() const {
    return m_module != nullptr;
}

} // namespace atlas::module
