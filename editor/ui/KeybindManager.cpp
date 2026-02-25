#include "KeybindManager.h"
#include <algorithm>

namespace atlas::editor {

// ── Construction ───────────────────────────────────────────────────

KeybindManager::KeybindManager() {
    InstallDefaults();
}

// ── Binding management ─────────────────────────────────────────────

bool KeybindManager::AddBinding(const Keybind& binding) {
    if (binding.action.empty()) return false;
    // Reject duplicate action names.
    if (findMutable(binding.action)) return false;
    m_bindings.push_back(binding);
    return true;
}

bool KeybindManager::RemoveBinding(const std::string& action) {
    auto it = std::remove_if(m_bindings.begin(), m_bindings.end(),
        [&action](const Keybind& kb) { return kb.action == action; });
    if (it == m_bindings.end()) return false;
    m_bindings.erase(it, m_bindings.end());
    // Also remove callback.
    m_callbacks.erase(
        std::remove_if(m_callbacks.begin(), m_callbacks.end(),
            [&action](const CallbackEntry& ce) { return ce.action == action; }),
        m_callbacks.end());
    return true;
}

bool KeybindManager::Rebind(const std::string& action, int key, KeyMod mods) {
    Keybind* kb = findMutable(action);
    if (!kb) return false;
    kb->key  = key;
    kb->mods = mods;
    return true;
}

bool KeybindManager::SetEnabled(const std::string& action, bool enabled) {
    Keybind* kb = findMutable(action);
    if (!kb) return false;
    kb->enabled = enabled;
    return true;
}

const Keybind* KeybindManager::FindBinding(const std::string& action) const {
    for (const auto& kb : m_bindings) {
        if (kb.action == action) return &kb;
    }
    return nullptr;
}

std::vector<const Keybind*> KeybindManager::FindByKey(int key, KeyMod mods) const {
    std::vector<const Keybind*> result;
    for (const auto& kb : m_bindings) {
        if (kb.key == key && kb.mods == mods) {
            result.push_back(&kb);
        }
    }
    return result;
}

std::vector<const Keybind*> KeybindManager::BindingsInCategory(
    const std::string& category) const {
    std::vector<const Keybind*> result;
    for (const auto& kb : m_bindings) {
        if (kb.category == category) {
            result.push_back(&kb);
        }
    }
    return result;
}

// ── Conflict detection ─────────────────────────────────────────────

bool KeybindManager::HasConflict(int key, KeyMod mods,
                                  const std::string& excludeAction) const {
    for (const auto& kb : m_bindings) {
        if (kb.key == key && kb.mods == mods && kb.action != excludeAction) {
            return true;
        }
    }
    return false;
}

// ── Action dispatch ────────────────────────────────────────────────

void KeybindManager::RegisterCallback(const std::string& action,
                                       ActionCallback callback) {
    // Replace existing callback or add new one.
    for (auto& ce : m_callbacks) {
        if (ce.action == action) {
            ce.callback = std::move(callback);
            return;
        }
    }
    m_callbacks.push_back({action, std::move(callback)});
}

bool KeybindManager::HandleKeyPress(int key, KeyMod mods) {
    for (const auto& kb : m_bindings) {
        if (kb.key == key && kb.mods == mods && kb.enabled) {
            for (const auto& ce : m_callbacks) {
                if (ce.action == kb.action && ce.callback) {
                    ce.callback();
                    return true;
                }
            }
        }
    }
    return false;
}

// ── Display helpers ────────────────────────────────────────────────

std::string KeybindManager::DescribeBinding(int key, KeyMod mods) {
    std::string desc;
    if (hasFlag(mods, KeyMod::Ctrl))  desc += "Ctrl+";
    if (hasFlag(mods, KeyMod::Shift)) desc += "Shift+";
    if (hasFlag(mods, KeyMod::Alt))   desc += "Alt+";

    // Common named keys.
    if (key == 127 || key == 8) desc += "Delete";
    else if (key == 27)         desc += "Escape";
    else if (key == 9)          desc += "Tab";
    else if (key == 13)         desc += "Enter";
    else if (key == 32)         desc += "Space";
    else if (key >= 'A' && key <= 'Z') desc += static_cast<char>(key);
    else if (key >= 'a' && key <= 'z') desc += static_cast<char>(key - 32); // uppercase
    else if (key >= '0' && key <= '9') desc += static_cast<char>(key);
    else desc += "Key(" + std::to_string(key) + ")";

    return desc;
}

std::string KeybindManager::DescribeAction(const std::string& action) const {
    const Keybind* kb = FindBinding(action);
    if (!kb) return action + " (unbound)";
    return action + " [" + DescribeBinding(kb->key, kb->mods) + "]";
}

// ── Default bindings ───────────────────────────────────────────────

void KeybindManager::InstallDefaults() {
    // General
    AddBinding({"Undo",       "General", 'Z', KeyMod::Ctrl});
    AddBinding({"Redo",       "General", 'Y', KeyMod::Ctrl});
    AddBinding({"Save",       "General", 'S', KeyMod::Ctrl});
    AddBinding({"Delete",     "General", 127, KeyMod::None});  // Delete key

    // Viewport
    AddBinding({"Translate",  "Viewport", 'W', KeyMod::None});
    AddBinding({"Rotate",     "Viewport", 'E', KeyMod::None});
    AddBinding({"Scale",      "Viewport", 'R', KeyMod::None});
    AddBinding({"ToggleGrid", "Viewport", 'G', KeyMod::None});
    AddBinding({"FocusSelected", "Viewport", 'F', KeyMod::None});

    // Panels
    AddBinding({"ToggleConsole",    "Panels", '`', KeyMod::None});
    AddBinding({"ToggleInspector",  "Panels", 'I', KeyMod::Ctrl});
    AddBinding({"ToggleSceneGraph", "Panels", 'H', KeyMod::Ctrl});
}

void KeybindManager::Clear() {
    m_bindings.clear();
    m_callbacks.clear();
}

// ── Internals ──────────────────────────────────────────────────────

Keybind* KeybindManager::findMutable(const std::string& action) {
    for (auto& kb : m_bindings) {
        if (kb.action == action) return &kb;
    }
    return nullptr;
}

} // namespace atlas::editor
