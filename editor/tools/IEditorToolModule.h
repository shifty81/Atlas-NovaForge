#pragma once
#include <cstdint>
#include <string>

namespace atlas::editor {

/// Abstract interface for editor tool modules.
///
/// Every editor tool (tile editor, modeling, animation, dialogue, etc.)
/// implements this interface.  The editor kernel discovers and loads
/// tool modules at startup, and standalone tool executables can load
/// a single module into a minimal editor shell.
///
/// This is the single most important editor architecture interface.
/// If a tool cannot be expressed through IEditorToolModule, the
/// interface must be extended — not bypassed.
class IEditorToolModule {
public:
    virtual ~IEditorToolModule() = default;

    /// Human-readable name of this tool (e.g. "Tile Editor").
    virtual std::string Name() const = 0;

    /// Called when the module is registered with the editor kernel.
    virtual void OnRegister() = 0;

    /// Called when the module is about to be unregistered.
    virtual void OnUnregister() = 0;

    /// Register any UI panels this tool provides.
    virtual void RegisterPanels() = 0;

    /// Register menu items this tool adds to the editor menu bar.
    virtual void RegisterMenus() = 0;

    /// Register editor modes this tool introduces.
    virtual void RegisterModes() = 0;

    /// Handle an input event.  Return true if the tool consumed it.
    virtual bool HandleInput(uint32_t keyCode, bool pressed) = 0;

    /// Per-frame update for the tool (called during editor tick).
    virtual void Update(float deltaTime) = 0;

    /// Called when the tool should render its viewport content.
    virtual void Render() = 0;
};

} // namespace atlas::editor
