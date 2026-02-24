#pragma once
#include "IGameModule.h"
#include <string>
#include <memory>

namespace atlas::module {

enum class ModuleLoadResult {
    Success,
    NotFound,
    SymbolMissing,
    AlreadyLoaded,
};

class ModuleLoader {
public:
    ModuleLoader();
    ~ModuleLoader();

    // Load a game module from a shared library path
    ModuleLoadResult Load(const std::string& path);

    // Unload the currently loaded module
    void Unload();

    // Set a statically linked module (for tests or static builds)
    void SetStaticModule(std::unique_ptr<IGameModule> mod);

    // Access the loaded module
    IGameModule* GetModule() const;
    bool IsLoaded() const;

private:
    std::unique_ptr<IGameModule> m_module;
    void* m_handle = nullptr;
};

} // namespace atlas::module
