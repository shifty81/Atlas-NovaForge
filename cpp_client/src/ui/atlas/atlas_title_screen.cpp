#include "ui/atlas/atlas_title_screen.h"
#include "ui/atlas/atlas_widgets.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace atlas {

AtlasTitleScreen::AtlasTitleScreen() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void AtlasTitleScreen::render(AtlasContext& ctx) {
    if (!m_active) return;

    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    // Full-screen background
    Rect bg(0.0f, 0.0f, windowW, windowH);
    r.drawRect(bg, Color(0.02f, 0.03f, 0.05f, 1.0f));

    // Sidebar strip (Atlas/Neocom style left edge)
    Rect sidebar(0.0f, 0.0f, SIDEBAR_WIDTH, windowH);
    r.drawRect(sidebar, theme.bgHeader);

    // Sidebar accent line
    r.drawRect(Rect(SIDEBAR_WIDTH - 1.0f, 0.0f, 1.0f, windowH),
               theme.accentSecondary.withAlpha(0.3f));

    // "A" logo in sidebar (Atlas Engine branding)
    float logoY = 16.0f;
    r.drawText("A", Vec2(SIDEBAR_WIDTH * 0.5f - 4.0f, logoY),
               theme.accentSecondary);

    // Render current page BEFORE consuming mouse so buttons can process clicks
    switch (m_currentPage) {
        case Page::MAIN:              renderMainMenu(ctx);          break;
        case Page::SETTINGS:          renderSettings(ctx);          break;
        case Page::CHARACTER_CREATION:renderCharacterCreation(ctx); break;
        case Page::SHIP_SELECTION:    renderShipSelection(ctx);     break;
        case Page::SHIP_LOADOUT:      renderShipLoadout(ctx);       break;
        case Page::HANGAR_SPAWN:      renderHangarSpawn(ctx);       break;
    }

    // Consume mouse AFTER widgets have processed input to prevent
    // click-through to the 3D scene behind the title screen
    if (ctx.isHovered(bg)) {
        ctx.consumeMouse();
    }
}

// ─────────────────────────────────────────────────────────────────
// Main Menu: New Game · Multiplayer · Settings · Quit
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderMainMenu(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    // Game title
    const char* titleLine1 = "NOVA FORGE";
    const char* titleLine2 = "A S T R A L I S";
    float title1W = r.measureText(titleLine1);
    float title2W = r.measureText(titleLine2);

    float titleY = windowH * 0.2f;
    r.drawText(titleLine1, Vec2(contentX + (contentW - title1W) * 0.5f, titleY),
               theme.accentSecondary);
    r.drawText(titleLine2, Vec2(contentX + (contentW - title2W) * 0.5f, titleY + 24.0f),
               theme.textSecondary);

    // Menu buttons — centered vertically below title
    float menuX = contentX + (contentW - MENU_WIDTH) * 0.5f;
    float menuY = windowH * 0.4f;

    // New Game
    Rect newGameBtn(menuX, menuY, MENU_WIDTH, BUTTON_HEIGHT);
    if (button(ctx, "New Game", newGameBtn)) {
        m_currentPage = Page::CHARACTER_CREATION;
        m_raceIdx = 0;
        m_bloodlineIdx = 0;
        m_careerIdx = 0;
    }
    menuY += BUTTON_HEIGHT + BUTTON_SPACING;

    // Multiplayer (placeholder)
    Rect mpBtn(menuX, menuY, MENU_WIDTH, BUTTON_HEIGHT);
    if (button(ctx, "Multiplayer", mpBtn)) {
        // TODO: multiplayer lobby / server browser
    }
    menuY += BUTTON_HEIGHT + BUTTON_SPACING;

    // Settings
    Rect settingsBtn(menuX, menuY, MENU_WIDTH, BUTTON_HEIGHT);
    if (button(ctx, "Settings", settingsBtn)) {
        m_currentPage = Page::SETTINGS;
    }
    menuY += BUTTON_HEIGHT + BUTTON_SPACING;

    // Quit
    Rect quitBtn(menuX, menuY, MENU_WIDTH, BUTTON_HEIGHT);
    if (button(ctx, "Quit", quitBtn)) {
        if (m_quitCb) m_quitCb();
    }

    // Version info at bottom
    const char* version = "Nova Forge v1.0.0  |  Atlas Engine";
    float verW = r.measureText(version);
    r.drawText(version, Vec2(contentX + (contentW - verW) * 0.5f, windowH - 30.0f),
               theme.textMuted);
}

// ─────────────────────────────────────────────────────────────────
// Settings (unchanged from original)
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderSettings(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    // Settings title
    const char* title = "SETTINGS";
    float titleW = r.measureText(title);
    float titleY = windowH * 0.15f;
    r.drawText(title, Vec2(contentX + (contentW - titleW) * 0.5f, titleY),
               theme.accentSecondary);

    // Settings panel centered
    float panelW = 400.0f;
    float panelX = contentX + (contentW - panelW) * 0.5f;
    float panelY = windowH * 0.25f;

    // Audio section
    label(ctx, Vec2(panelX, panelY), "Audio", theme.accentSecondary);
    panelY += 24.0f;

    // Master Volume
    label(ctx, Vec2(panelX, panelY), "Master Volume", theme.textSecondary);
    panelY += 18.0f;
    Rect masterSlider(panelX, panelY, panelW, 20.0f);
    slider(ctx, "title_master_vol", masterSlider, &m_masterVolume, 0.0f, 1.0f, "%.0f%%");
    panelY += 20.0f + BUTTON_SPACING;

    // Music Volume
    label(ctx, Vec2(panelX, panelY), "Music Volume", theme.textSecondary);
    panelY += 18.0f;
    Rect musicSlider(panelX, panelY, panelW, 20.0f);
    slider(ctx, "title_music_vol", musicSlider, &m_musicVolume, 0.0f, 1.0f, "%.0f%%");
    panelY += 20.0f + BUTTON_SPACING;

    // SFX Volume
    label(ctx, Vec2(panelX, panelY), "SFX Volume", theme.textSecondary);
    panelY += 18.0f;
    Rect sfxSlider(panelX, panelY, panelW, 20.0f);
    slider(ctx, "title_sfx_vol", sfxSlider, &m_sfxVolume, 0.0f, 1.0f, "%.0f%%");
    panelY += 20.0f + BUTTON_SPACING * 2.0f;

    // Back button
    float btnW = 200.0f;
    float btnX = contentX + (contentW - btnW) * 0.5f;
    Rect backBtn(btnX, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Back", backBtn)) {
        m_currentPage = Page::MAIN;
    }
}

// ─────────────────────────────────────────────────────────────────
// Character Creation: race → bloodline → career
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderCharacterCreation(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    // Title
    const char* title = "CHARACTER CREATION";
    float titleW = r.measureText(title);
    r.drawText(title, Vec2(contentX + (contentW - titleW) * 0.5f, windowH * 0.1f),
               theme.accentSecondary);

    float panelW = 400.0f;
    float panelX = contentX + (contentW - panelW) * 0.5f;
    float panelY = windowH * 0.2f;

    // ── Race selection ──────────────────────────────────────────
    label(ctx, Vec2(panelX, panelY), "Race", theme.accentSecondary);
    panelY += 24.0f;

    for (int i = 0; i < static_cast<int>(m_races.size()); ++i) {
        Rect btn(panelX, panelY, panelW, BUTTON_HEIGHT);
        const char* name = m_races[i].name.c_str();
        bool selected = (i == m_raceIdx);

        if (selected) {
            r.drawRect(btn, theme.accentSecondary.withAlpha(0.15f));
        }
        if (button(ctx, name, btn)) {
            m_raceIdx = i;
        }
        panelY += BUTTON_HEIGHT + 4.0f;
    }

    panelY += BUTTON_SPACING;

    // ── Career selection ────────────────────────────────────────
    label(ctx, Vec2(panelX, panelY), "Career", theme.accentSecondary);
    panelY += 24.0f;

    for (int i = 0; i < static_cast<int>(m_careers.size()); ++i) {
        Rect btn(panelX, panelY, panelW, BUTTON_HEIGHT);
        bool selected = (i == m_careerIdx);

        if (selected) {
            r.drawRect(btn, theme.accentSecondary.withAlpha(0.15f));
        }
        if (button(ctx, m_careers[i].name.c_str(), btn)) {
            m_careerIdx = i;
        }

        // Show description for selected career
        if (selected) {
            panelY += BUTTON_HEIGHT + 2.0f;
            label(ctx, Vec2(panelX + 8.0f, panelY),
                  m_careers[i].desc.c_str(), theme.textMuted);
            panelY += 16.0f;
        } else {
            panelY += BUTTON_HEIGHT + 4.0f;
        }
    }

    panelY += BUTTON_SPACING;

    // ── Navigation ──────────────────────────────────────────────
    float btnW = 150.0f;
    float gap  = 20.0f;
    float totalW = btnW * 2 + gap;
    float startX = contentX + (contentW - totalW) * 0.5f;

    Rect backBtn(startX, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Back", backBtn)) {
        m_currentPage = Page::MAIN;
    }

    Rect nextBtn(startX + btnW + gap, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Next", nextBtn)) {
        m_selectedRace      = m_races[m_raceIdx].id;
        m_selectedCareer    = m_careers[m_careerIdx].id;
        resolveStarterShip();
        m_currentPage = Page::SHIP_SELECTION;
    }
}

// ─────────────────────────────────────────────────────────────────
// Ship Selection: shows the resolved starter ship
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderShipSelection(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    // Title
    const char* title = "STARTER SHIP ASSIGNMENT";
    float titleW = r.measureText(title);
    r.drawText(title, Vec2(contentX + (contentW - titleW) * 0.5f, windowH * 0.1f),
               theme.accentSecondary);

    float panelW = 420.0f;
    float panelX = contentX + (contentW - panelW) * 0.5f;
    float panelY = windowH * 0.22f;

    // Career summary
    label(ctx, Vec2(panelX, panelY), "Career:", theme.accentSecondary);
    label(ctx, Vec2(panelX + 80.0f, panelY), m_selectedCareer.c_str(), theme.textPrimary);
    panelY += 28.0f;

    label(ctx, Vec2(panelX, panelY), "Race:", theme.accentSecondary);
    label(ctx, Vec2(panelX + 80.0f, panelY), m_selectedRace.c_str(), theme.textPrimary);
    panelY += 28.0f;

    // Assigned ship class
    label(ctx, Vec2(panelX, panelY), "Ship Class:", theme.accentSecondary);
    panelY += 24.0f;

    // Ship class display box
    Rect shipBox(panelX, panelY, panelW, 80.0f);
    r.drawRect(shipBox, theme.bgPanel);
    r.drawRect(Rect(panelX, panelY, panelW, 1.0f), theme.accentSecondary.withAlpha(0.4f));

    std::string shipLabel;
    std::string shipDesc;

    if (m_selectedShipClass == "destroyer") {
        shipLabel = "DESTROYER";
        shipDesc  = "Heavy hull - 7 high slots, 450 cargo capacity";
    } else {
        shipLabel = "FRIGATE";
        shipDesc  = "Light hull - 3 high slots, 250 cargo capacity";
    }

    float shipLabelW = r.measureText(shipLabel.c_str());
    r.drawText(shipLabel.c_str(),
               Vec2(panelX + (panelW - shipLabelW) * 0.5f, panelY + 20.0f),
               theme.accentSecondary);

    float descW = r.measureText(shipDesc.c_str());
    r.drawText(shipDesc.c_str(),
               Vec2(panelX + (panelW - descW) * 0.5f, panelY + 48.0f),
               theme.textSecondary);

    panelY += 80.0f + BUTTON_SPACING;

    // Industrialist note
    if (m_selectedCareer == "industrialist") {
        const char* note = "Industrialists always receive a destroyer hull.";
        float noteW = r.measureText(note);
        r.drawText(note, Vec2(panelX + (panelW - noteW) * 0.5f, panelY),
                   theme.textMuted);
        panelY += 24.0f;
    }

    panelY += BUTTON_SPACING;

    // Navigation
    float btnW = 150.0f;
    float gap  = 20.0f;
    float totalW = btnW * 2 + gap;
    float startX = contentX + (contentW - totalW) * 0.5f;

    Rect backBtn(startX, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Back", backBtn)) {
        m_currentPage = Page::CHARACTER_CREATION;
    }

    Rect nextBtn(startX + btnW + gap, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Configure Ship", nextBtn)) {
        // Set up loadout slots based on ship class
        if (m_selectedShipClass == "destroyer") {
            m_shipDisplayName  = "Starter Destroyer";
            m_loadoutHighSlots = 7;
            m_loadoutMidSlots  = 3;
            m_loadoutLowSlots  = 3;
        } else {
            m_shipDisplayName  = "Starter Frigate";
            m_loadoutHighSlots = 3;
            m_loadoutMidSlots  = 3;
            m_loadoutLowSlots  = 3;
        }
        m_currentPage = Page::SHIP_LOADOUT;
    }
}

// ─────────────────────────────────────────────────────────────────
// Ship Loadout: configure starting modules
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderShipLoadout(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    const char* title = "SHIP LOADOUT";
    float titleW = r.measureText(title);
    r.drawText(title, Vec2(contentX + (contentW - titleW) * 0.5f, windowH * 0.1f),
               theme.accentSecondary);

    float panelW = 420.0f;
    float panelX = contentX + (contentW - panelW) * 0.5f;
    float panelY = windowH * 0.2f;

    // Ship name
    label(ctx, Vec2(panelX, panelY), m_shipDisplayName.c_str(), theme.textPrimary);
    panelY += 28.0f;

    // Slot summary
    char slotText[128];
    std::snprintf(slotText, sizeof(slotText),
                  "High: %d   Mid: %d   Low: %d",
                  m_loadoutHighSlots, m_loadoutMidSlots, m_loadoutLowSlots);
    label(ctx, Vec2(panelX, panelY), slotText, theme.textSecondary);
    panelY += 28.0f;

    // Slot placeholders
    auto drawSlotRow = [&](const char* name, int count) {
        label(ctx, Vec2(panelX, panelY), name, theme.accentSecondary);
        panelY += 20.0f;
        for (int i = 0; i < count; ++i) {
            Rect slot(panelX + static_cast<float>(i) * 52.0f, panelY, 48.0f, 32.0f);
            r.drawRect(slot, theme.bgPanel);
            r.drawRect(Rect(slot.x, slot.y, slot.w, 1.0f),
                       theme.accentSecondary.withAlpha(0.3f));
            label(ctx, Vec2(slot.x + 8.0f, slot.y + 8.0f), "---", theme.textMuted);
        }
        panelY += 32.0f + 12.0f;
    };

    drawSlotRow("High Slots", m_loadoutHighSlots);
    drawSlotRow("Mid Slots",  m_loadoutMidSlots);
    drawSlotRow("Low Slots",  m_loadoutLowSlots);

    panelY += BUTTON_SPACING;

    // Navigation
    float btnW = 150.0f;
    float gap  = 20.0f;
    float totalW = btnW * 2 + gap;
    float startX = contentX + (contentW - totalW) * 0.5f;

    Rect backBtn(startX, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Back", backBtn)) {
        m_currentPage = Page::SHIP_SELECTION;
    }

    Rect nextBtn(startX + btnW + gap, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Launch", nextBtn)) {
        m_currentPage = Page::HANGAR_SPAWN;
    }
}

// ─────────────────────────────────────────────────────────────────
// Hangar Spawn: you start on foot in a station hangar
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::renderHangarSpawn(AtlasContext& ctx) {
    const auto& theme = ctx.theme();
    auto& r = ctx.renderer();
    float windowW = static_cast<float>(ctx.input().windowW);
    float windowH = static_cast<float>(ctx.input().windowH);

    float contentX = SIDEBAR_WIDTH;
    float contentW = windowW - contentX;

    const char* title = "STATION HANGAR";
    float titleW = r.measureText(title);
    r.drawText(title, Vec2(contentX + (contentW - titleW) * 0.5f, windowH * 0.15f),
               theme.accentSecondary);

    float panelW = 480.0f;
    float panelX = contentX + (contentW - panelW) * 0.5f;
    float panelY = windowH * 0.28f;

    // Flavour text
    const char* desc1 = "You awake in a dimly-lit station hangar.";
    const char* desc2 = "Your ship sits on the landing pad, ready to launch.";
    const char* desc3 = "Look around — there may be supplies worth grabbing.";
    label(ctx, Vec2(panelX, panelY), desc1, theme.textSecondary); panelY += 22.0f;
    label(ctx, Vec2(panelX, panelY), desc2, theme.textSecondary); panelY += 22.0f;
    label(ctx, Vec2(panelX, panelY), desc3, theme.textSecondary); panelY += 36.0f;

    const char* hint1 = "• Explore the hangar on foot (FPS mode)";
    const char* hint2 = "• Loot anything lying around";
    const char* hint3 = "• Board your ship to access the cockpit";
    const char* hint4 = "• Use the Fleet Command Console for strategy view";
    label(ctx, Vec2(panelX, panelY), hint1, theme.textMuted); panelY += 20.0f;
    label(ctx, Vec2(panelX, panelY), hint2, theme.textMuted); panelY += 20.0f;
    label(ctx, Vec2(panelX, panelY), hint3, theme.textMuted); panelY += 20.0f;
    label(ctx, Vec2(panelX, panelY), hint4, theme.textMuted); panelY += 36.0f;

    // Undock button
    float btnW = 240.0f;
    float btnX = contentX + (contentW - btnW) * 0.5f;
    Rect undockBtn(btnX, panelY, btnW, BUTTON_HEIGHT);
    if (button(ctx, "Enter Hangar", undockBtn)) {
        m_active = false;
        if (m_playCb) m_playCb();
    }
}

// ─────────────────────────────────────────────────────────────────
// Resolve starter ship class based on career
// ─────────────────────────────────────────────────────────────────
void AtlasTitleScreen::resolveStarterShip() {
    if (m_selectedCareer == "industrialist") {
        // Industrialists always start with a destroyer hull
        m_selectedShipClass = "destroyer";
    } else {
        // All other careers: random roll (60% frigate, 40% destroyer)
        int roll = std::rand() % 100;
        m_selectedShipClass = (roll < 60) ? "frigate" : "destroyer";
    }
    std::cout << "[CharGen] Career=" << m_selectedCareer
              << " => StarterShip=" << m_selectedShipClass << std::endl;
}

} // namespace atlas
