#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <algorithm>

namespace atlas {

enum class GamePhase : uint8_t {
    MainMenu,
    Loading,
    InSpace,
    Docked,
    InWarp,
    Paused
};

inline const char* GamePhaseName(GamePhase phase) {
    switch (phase) {
        case GamePhase::MainMenu: return "MainMenu";
        case GamePhase::Loading:  return "Loading";
        case GamePhase::InSpace:  return "InSpace";
        case GamePhase::Docked:   return "Docked";
        case GamePhase::InWarp:   return "InWarp";
        case GamePhase::Paused:   return "Paused";
    }
    return "Unknown";
}

class GameStateManager {
public:
    using PhaseCallback = std::function<void(GamePhase, GamePhase)>;

    GamePhase Phase() const { return m_phase; }
    void SetPhase(GamePhase p) {
        if (p == m_phase) return;
        GamePhase old = m_phase;
        m_phase = p;
        for (auto& cb : m_callbacks) cb(old, p);
    }
    void OnPhaseChange(PhaseCallback cb) { m_callbacks.push_back(std::move(cb)); }

    uint64_t PlayerEntity() const { return m_player; }
    void SetPlayerEntity(uint64_t v) { m_player = v; }

    uint64_t CurrentSystem() const { return m_system; }
    void SetCurrentSystem(uint64_t v) { m_system = v; }

    uint64_t DockedStation() const { return m_station; }
    void SetDockedStation(uint64_t v) { m_station = v; }

    int64_t Credits() const { return m_credits; }
    void SetCredits(int64_t v) { m_credits = v; }
    void AddCredits(int64_t v) { m_credits += v; }
    bool CanAfford(int64_t cost) const { return m_credits >= cost; }
    bool Spend(int64_t cost) {
        if (!CanAfford(cost)) return false;
        m_credits -= cost;
        return true;
    }

    size_t InventorySize() const { return m_inventory.size(); }
    bool HasItem(uint64_t id) const {
        return std::find(m_inventory.begin(), m_inventory.end(), id) != m_inventory.end();
    }
    void AddItem(uint64_t id) { m_inventory.push_back(id); }
    bool RemoveItem(uint64_t id) {
        auto it = std::find(m_inventory.begin(), m_inventory.end(), id);
        if (it == m_inventory.end()) return false;
        m_inventory.erase(it);
        return true;
    }

    void Reset() {
        m_phase = GamePhase::MainMenu;
        m_player = 0;
        m_system = 0;
        m_station = 0;
        m_credits = 0;
        m_inventory.clear();
    }

private:
    GamePhase m_phase = GamePhase::MainMenu;
    uint64_t m_player = 0;
    uint64_t m_system = 0;
    uint64_t m_station = 0;
    int64_t m_credits = 0;
    std::vector<uint64_t> m_inventory;
    std::vector<PhaseCallback> m_callbacks;
};

} // namespace atlas
