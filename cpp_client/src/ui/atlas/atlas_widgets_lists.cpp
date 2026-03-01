#include "ui/atlas/atlas_widgets.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static constexpr float METERS_PER_AU = 149597870700.0f;

namespace atlas {

// Helper: truncate a string with "..." suffix if it exceeds maxWidth pixels
static std::string truncateText(AtlasRenderer& rr, const std::string& text, float maxWidth) {
    if (text.empty() || rr.measureText(text) <= maxWidth) return text;
    std::string result = text;
    while (result.size() > 1 && rr.measureText(result + "...") > maxWidth) {
        result.pop_back();
    }
    result += "...";
    return result;
}

// ── Overview ────────────────────────────────────────────────────────

void overviewHeader(AtlasContext& ctx, const Rect& r,
                    const std::vector<std::string>& tabs,
                    int activeTab) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    // Header background
    rr.drawRect(r, t.bgHeader);

    // Tabs
    float tabX = r.x + 4.0f;
    float tabH = r.h - 4.0f;
    for (int i = 0; i < static_cast<int>(tabs.size()); ++i) {
        float tw = rr.measureText(tabs[i]) + 16.0f;
        Rect tabRect = {tabX, r.y + 2.0f, tw, tabH};

        if (i == activeTab) {
            rr.drawRect(tabRect, t.selection);
            rr.drawText(tabs[i], {tabX + 8.0f, r.y + 5.0f}, t.textPrimary);
        } else {
            rr.drawText(tabs[i], {tabX + 8.0f, r.y + 5.0f}, t.textSecondary);
        }
        tabX += tw + 2.0f;
    }

    // Bottom border
    rr.drawRect({r.x, r.bottom() - 1.0f, r.w, 1.0f}, t.borderSubtle);
}

bool overviewRow(AtlasContext& ctx, const Rect& r,
                 const OverviewEntry& entry, bool isAlternate) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = hashID(entry.name.c_str());
    bool clicked = ctx.buttonBehavior(r, id);

    // Photon-style: neutral row background (alternating for scanability)
    Color bg = isAlternate ? t.bgSecondary.withAlpha(0.2f)
                           : Color(0, 0, 0, 0);
    if (ctx.isHot(id)) bg = t.hover;
    rr.drawRect(r, bg);

    // Photon selection: thin 2px accent bar on the left (not row fill)
    if (entry.selected) {
        rr.drawRect({r.x, r.y, t.selectionBarWidth, r.h}, t.accentPrimary);
    }

    // Standing/threat icon indicator (small square — color the icon, not the row)
    float iconEnd = r.x + 16.0f;
    rr.drawRect({r.x + 4.0f, r.y + (r.h - 8.0f) * 0.5f, 8.0f, 8.0f},
                entry.standingColor);

    // Columns: Distance | Name | Type — proportional to row width
    float textY = r.y + (r.h - 13.0f) * 0.5f;
    float usableW = r.w - 16.0f;  // subtract icon area
    float distColW = usableW * 0.28f;
    float nameColW = usableW * 0.40f;
    float typeColW = usableW * 0.32f;

    float distColStart = iconEnd;
    float nameColStart = distColStart + distColW;
    float typeColStart = nameColStart + nameColW;

    // Distance (right-aligned in its column for readability)
    char distBuf[32];
    if (entry.distance >= METERS_PER_AU * 0.01f) {
        std::snprintf(distBuf, sizeof(distBuf), "%.1f AU", entry.distance / METERS_PER_AU);
    } else if (entry.distance >= 1000.0f) {
        std::snprintf(distBuf, sizeof(distBuf), "%.0f km", entry.distance / 1000.0f);
    } else {
        std::snprintf(distBuf, sizeof(distBuf), "%.0f m", entry.distance);
    }
    float distW = rr.measureText(distBuf);
    float distEnd = nameColStart - 4.0f;
    rr.drawText(distBuf, {distEnd - distW, textY}, t.textSecondary);

    // Name — truncate with "..." if it exceeds column width
    float nameMaxW = nameColW - 6.0f;
    std::string displayName = truncateText(rr, entry.name, nameMaxW);
    rr.drawText(displayName, {nameColStart + 2.0f, textY}, t.textPrimary);

    // Type — truncate with "..." if it exceeds column width
    float typeMaxW = typeColW - 4.0f;
    std::string displayType = truncateText(rr, entry.type, typeMaxW);
    rr.drawText(displayType, {typeColStart + 2.0f, textY}, t.textSecondary);

    return clicked;
}

// ── Overview Header Interactive ─────────────────────────────────────

int overviewHeaderInteractive(AtlasContext& ctx, const Rect& r,
                              const std::vector<std::string>& tabs,
                              int activeTab) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    int clickedTab = -1;

    // Header background
    rr.drawRect(r, t.bgHeader);

    // Photon-style tabs: text-only with accent underline on active
    float tabX = r.x + 4.0f;
    float tabH = r.h - 4.0f;
    for (int i = 0; i < static_cast<int>(tabs.size()); ++i) {
        float tw = rr.measureText(tabs[i]) + 16.0f;
        Rect tabRect = {tabX, r.y + 2.0f, tw, tabH};

        WidgetID tabID = hashID("ovtab") ^ static_cast<uint32_t>(i);
        bool clicked = ctx.buttonBehavior(tabRect, tabID);
        if (clicked) clickedTab = i;

        bool hovered = ctx.isHot(tabID);

        if (i == activeTab) {
            // Active tab: bright text + accent underline indicator
            rr.drawText(tabs[i], {tabX + 8.0f, r.y + 5.0f}, t.textPrimary);
            rr.drawRect({tabX + 4.0f, tabRect.bottom() - 2.0f, tw - 8.0f, 2.0f},
                        t.accentPrimary);
        } else if (hovered) {
            rr.drawRect(tabRect, t.hover);
            rr.drawText(tabs[i], {tabX + 8.0f, r.y + 5.0f}, t.textPrimary);
        } else {
            rr.drawText(tabs[i], {tabX + 8.0f, r.y + 5.0f}, t.textSecondary);
        }
        tabX += tw + 2.0f;
    }

    // Bottom border
    rr.drawRect({r.x, r.bottom() - 1.0f, r.w, 1.0f}, t.borderSubtle);

    return clickedTab;
}

// ── Tab Bar ─────────────────────────────────────────────────────────

int tabBar(AtlasContext& ctx, const Rect& r,
           const std::vector<std::string>& labels, int activeIdx) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    int clicked = -1;

    // Background
    rr.drawRect(r, t.bgHeader);

    float tabX = r.x + 4.0f;
    float tabH = r.h - 4.0f;
    for (int i = 0; i < static_cast<int>(labels.size()); ++i) {
        float tw = rr.measureText(labels[i]) + 16.0f;
        Rect tabRect = {tabX, r.y + 2.0f, tw, tabH};

        WidgetID tabID = hashID("tab") ^ static_cast<uint32_t>(i);
        if (ctx.buttonBehavior(tabRect, tabID)) clicked = i;

        bool hovered = ctx.isHot(tabID);

        if (i == activeIdx) {
            rr.drawText(labels[i], {tabX + 8.0f, r.y + 5.0f}, t.textPrimary);
            rr.drawRect({tabX + 4.0f, tabRect.bottom() - 2.0f, tw - 8.0f, 2.0f},
                        t.accentPrimary);
        } else if (hovered) {
            rr.drawRect(tabRect, t.hover);
            rr.drawText(labels[i], {tabX + 8.0f, r.y + 5.0f}, t.textPrimary);
        } else {
            rr.drawText(labels[i], {tabX + 8.0f, r.y + 5.0f}, t.textSecondary);
        }
        tabX += tw + 2.0f;
    }

    rr.drawRect({r.x, r.bottom() - 1.0f, r.w, 1.0f}, t.borderSubtle);
    return clicked;
}

// ── Combat Log Widget ───────────────────────────────────────────────

void combatLogWidget(AtlasContext& ctx, const Rect& r,
                     const std::vector<std::string>& messages,
                     float& scrollOff, int maxVisible) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    // Panel background
    rr.drawRect(r, t.bgPanel);
    rr.drawRectOutline(r, t.borderSubtle, t.borderWidth);

    // Header
    float hh = 18.0f;
    rr.drawRect({r.x, r.y, r.w, hh}, t.bgHeader);
    rr.drawText("Combat Log", {r.x + 6.0f, r.y + 3.0f}, t.textSecondary);

    // Content area
    float contentY = r.y + hh + 2.0f;
    float contentH = r.h - hh - 2.0f;
    float rowH = 16.0f;
    int visRows = maxVisible > 0 ? maxVisible : static_cast<int>(contentH / rowH);
    if (visRows <= 0) visRows = 1;

    // Handle scrolling
    if (ctx.isHovered(r)) {
        scrollOff -= ctx.input().scrollY * rowH * 2.0f;
    }

    int total = static_cast<int>(messages.size());
    float maxScroll = std::max(0.0f, (total - visRows) * rowH);
    if (scrollOff < 0.0f) scrollOff = 0.0f;
    if (scrollOff > maxScroll) scrollOff = maxScroll;

    // Draw visible messages (newest at bottom)
    int firstRow = static_cast<int>(scrollOff / rowH);
    for (int i = 0; i < visRows + 1 && (firstRow + i) < total; ++i) {
        int msgIdx = firstRow + i;
        float y = contentY + i * rowH - std::fmod(scrollOff, rowH);
        if (y + rowH < contentY || y > r.bottom()) continue;

        // Fade older messages
        float age = static_cast<float>(total - 1 - msgIdx) / std::max(1.0f, static_cast<float>(total));
        float alpha = 1.0f - age * 0.4f;
        Color textCol = t.textPrimary.withAlpha(alpha);

        rr.drawText(messages[msgIdx], {r.x + 6.0f, y + 1.0f}, textCol);
    }

    // Scrollbar
    if (total > visRows) {
        Rect scrollTrack = {r.right() - t.scrollbarWidth, contentY,
                           t.scrollbarWidth, contentH};
        scrollbar(ctx, scrollTrack, scrollOff, total * rowH, contentH);
    }
}

} // namespace atlas
