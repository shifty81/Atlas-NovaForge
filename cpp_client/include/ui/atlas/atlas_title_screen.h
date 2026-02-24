#pragma once

/**
 * @file atlas_title_screen.h
 * @brief Title/main menu screen for the Atlas Engine
 *
 * Shown on application startup before entering the game. Provides a full
 * menu flow:
 *   - Main Menu: New Game, Multiplayer, Settings, Quit
 *   - Character Creation: race, bloodline, career selection
 *   - Ship Selection: starter ship (shuttle/frigate/destroyer) based on career
 *   - Ship Loadout: configure starting modules
 *   - Hangar Spawn: enter the game on foot in a station hangar
 *
 * Industrialists always receive a destroyer hull. Other careers roll for
 * frigate or destroyer. Strategy mode is accessed via the fleet command
 * console once in-game.
 *
 * Styled consistently with the Atlas UI Photon Dark theme, using the
 * same sidebar-inspired layout as the in-game UI.
 */

#include "atlas_context.h"
#include <string>
#include <functional>
#include <vector>

namespace atlas {

class AtlasTitleScreen {
public:
    AtlasTitleScreen();
    ~AtlasTitleScreen() = default;

    // ── State ───────────────────────────────────────────────────────

    /** Check if the title screen is active (should be shown). */
    bool isActive() const { return m_active; }

    /** Set whether the title screen is active. */
    void setActive(bool active) { m_active = active; }

    // ── Rendering ───────────────────────────────────────────────────

    /** Render the title screen. Call between beginFrame/endFrame. */
    void render(AtlasContext& ctx);

    // ── Callbacks ───────────────────────────────────────────────────

    /** Set callback for Play button (enters the game / hangar spawn). */
    void setPlayCallback(std::function<void()> cb) { m_playCb = std::move(cb); }

    /** Set callback for Quit button. */
    void setQuitCallback(std::function<void()> cb) { m_quitCb = std::move(cb); }

    // ── Settings access (shared with pause menu) ────────────────────

    float getMasterVolume() const { return m_masterVolume; }
    void setMasterVolume(float v) { m_masterVolume = v; }

    float getMusicVolume() const { return m_musicVolume; }
    void setMusicVolume(float v) { m_musicVolume = v; }

    float getSfxVolume() const { return m_sfxVolume; }
    void setSfxVolume(float v) { m_sfxVolume = v; }

    /** Check if the title screen wants keyboard input. */
    bool wantsKeyboardInput() const { return m_active; }

    // ── Character / ship query (read after flow completes) ──────────

    const std::string& getSelectedRace()      const { return m_selectedRace; }
    const std::string& getSelectedBloodline() const { return m_selectedBloodline; }
    const std::string& getSelectedCareer()    const { return m_selectedCareer; }
    const std::string& getSelectedShipClass() const { return m_selectedShipClass; }

private:
    // ── Page renderers ──────────────────────────────────────────────
    void renderMainMenu(AtlasContext& ctx);
    void renderSettings(AtlasContext& ctx);
    void renderCharacterCreation(AtlasContext& ctx);
    void renderShipSelection(AtlasContext& ctx);
    void renderShipLoadout(AtlasContext& ctx);
    void renderHangarSpawn(AtlasContext& ctx);

    /** Determine starter ship class based on selected career. */
    void resolveStarterShip();

    bool m_active = true;

    enum class Page {
        MAIN,
        SETTINGS,
        CHARACTER_CREATION,
        SHIP_SELECTION,
        SHIP_LOADOUT,
        HANGAR_SPAWN
    };
    Page m_currentPage = Page::MAIN;

    // ── Character creation state ────────────────────────────────────
    struct RaceOption  { std::string id; std::string name; };
    struct CareerOption { std::string id; std::string name; std::string desc; };

    std::vector<RaceOption>    m_races = {
        {"solari",   "Solari"},
        {"veyren",   "Veyren"},
        {"aurelian", "Aurelian"},
        {"keldari",  "Keldari"}
    };
    std::vector<std::string> m_bloodlines;   // populated per race
    std::vector<CareerOption> m_careers = {
        {"soldier",       "Soldier",       "Combat specialist"},
        {"explorer",      "Explorer",      "Scanner & pathfinder"},
        {"industrialist", "Industrialist", "Builder & producer"},
        {"trader",        "Trader",        "Commerce & logistics"}
    };

    int m_raceIdx      = 0;
    int m_bloodlineIdx = 0;
    int m_careerIdx    = 0;

    std::string m_selectedRace;
    std::string m_selectedBloodline;
    std::string m_selectedCareer;
    std::string m_selectedShipClass;   // "frigate" or "destroyer"

    // ── Ship loadout state ──────────────────────────────────────────
    std::string m_shipDisplayName;
    int m_loadoutHighSlots = 0;
    int m_loadoutMidSlots  = 0;
    int m_loadoutLowSlots  = 0;

    // ── Audio settings ──────────────────────────────────────────────
    float m_masterVolume = 0.8f;
    float m_musicVolume = 0.5f;
    float m_sfxVolume = 0.7f;

    // ── Callbacks ───────────────────────────────────────────────────
    std::function<void()> m_playCb;
    std::function<void()> m_quitCb;

    // ── Visual constants ────────────────────────────────────────────
    static constexpr float SIDEBAR_WIDTH = 56.0f;
    static constexpr float MENU_WIDTH = 320.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
    static constexpr float BUTTON_SPACING = 14.0f;
    static constexpr float PADDING = 24.0f;
};

} // namespace atlas
