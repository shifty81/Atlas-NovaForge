#include "ui/atlas/atlas_widgets.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static constexpr float METERS_PER_AU = 149597870700.0f;

namespace atlas {

// ── Utility Widgets ─────────────────────────────────────────────────

void label(AtlasContext& ctx, Vec2 pos, const std::string& text,
           const Color& color) {
    const Theme& t = ctx.theme();
    Color c = (color.a > 0.01f) ? color : t.textPrimary;
    ctx.renderer().drawText(text, pos, c);
}

void separator(AtlasContext& ctx, Vec2 start, float width) {
    const Theme& t = ctx.theme();
    ctx.renderer().drawRect({start.x, start.y, width, 1.0f}, t.borderSubtle);
}

bool treeNode(AtlasContext& ctx, const Rect& r,
              const char* nodeLabel, bool* expanded) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = ctx.currentID(nodeLabel);
    bool clicked = ctx.buttonBehavior(r, id);

    if (ctx.isHot(id)) {
        rr.drawRect(r, t.hover);
    }

    // Triangle indicator
    float triX = r.x + 4.0f;
    float triY = r.y + r.h * 0.5f;
    if (expanded && *expanded) {
        // Down-pointing triangle (▼)
        rr.drawLine({triX, triY - 4.0f}, {triX + 8.0f, triY - 4.0f},
                    t.textSecondary, 1.0f);
        rr.drawLine({triX, triY - 4.0f}, {triX + 4.0f, triY + 4.0f},
                    t.textSecondary, 1.0f);
        rr.drawLine({triX + 8.0f, triY - 4.0f}, {triX + 4.0f, triY + 4.0f},
                    t.textSecondary, 1.0f);
    } else {
        // Right-pointing triangle (▶)
        rr.drawLine({triX, triY - 5.0f}, {triX, triY + 5.0f},
                    t.textSecondary, 1.0f);
        rr.drawLine({triX, triY - 5.0f}, {triX + 8.0f, triY},
                    t.textSecondary, 1.0f);
        rr.drawLine({triX, triY + 5.0f}, {triX + 8.0f, triY},
                    t.textSecondary, 1.0f);
    }

    // Label text
    rr.drawText(nodeLabel, {r.x + 16.0f, r.y + (r.h - 13.0f) * 0.5f},
                t.textPrimary);

    if (clicked && expanded) {
        *expanded = !(*expanded);
    }
    return expanded ? *expanded : false;
}

void scrollbar(AtlasContext& ctx, const Rect& track,
               float scrollOffset, float contentHeight, float viewHeight) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    if (contentHeight <= viewHeight) return;

    // Track background
    rr.drawRect(track, t.bgSecondary.withAlpha(0.3f));

    // Thumb
    float thumbRatio = viewHeight / contentHeight;
    float thumbH = std::max(20.0f, track.h * thumbRatio);
    float scrollRange = contentHeight - viewHeight;
    float thumbOffset = (scrollRange > 0.0f)
        ? (scrollOffset / scrollRange) * (track.h - thumbH) : 0.0f;

    Rect thumb = {track.x, track.y + thumbOffset, track.w, thumbH};
    rr.drawRect(thumb, t.accentDim);
}

// ── Tooltip ─────────────────────────────────────────────────────────

void tooltip(AtlasContext& ctx, const std::string& text) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    float tw = rr.measureText(text) + 16.0f;
    float th = 24.0f;
    Vec2 mouse = ctx.input().mousePos;
    // Position tooltip slightly below and to the right of cursor
    Rect tipRect = {mouse.x + 12.0f, mouse.y + 16.0f, tw, th};

    // Clamp to window bounds
    float winW = static_cast<float>(ctx.input().windowW);
    float winH = static_cast<float>(ctx.input().windowH);
    if (tipRect.right() > winW) tipRect.x = winW - tw;
    if (tipRect.bottom() > winH) tipRect.y = mouse.y - th - 4.0f;

    rr.drawRect(tipRect, t.bgTooltip);
    rr.drawRectOutline(tipRect, t.borderNormal);
    rr.drawText(text, {tipRect.x + 8.0f, tipRect.y + 5.0f}, t.textPrimary);
}

// ── Notification Toast ──────────────────────────────────────────────

void notification(AtlasContext& ctx, const std::string& text,
                  const Color& color) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    float winW = static_cast<float>(ctx.input().windowW);
    float notifW = rr.measureText(text) + 32.0f;
    float notifH = 28.0f;
    float notifX = (winW - notifW) * 0.5f;
    float notifY = 40.0f;

    // Background
    rr.drawRect({notifX, notifY, notifW, notifH}, t.bgPanel);
    rr.drawRectOutline({notifX, notifY, notifW, notifH}, t.borderSubtle);

    // Left accent bar
    Color accentCol = (color.a > 0.01f) ? color : t.accentPrimary;
    rr.drawRect({notifX, notifY, 3.0f, notifH}, accentCol);

    // Text
    float textY = notifY + (notifH - 13.0f) * 0.5f;
    rr.drawText(text, {notifX + 12.0f, textY}, t.textPrimary);
}

// ── Mode Indicator ──────────────────────────────────────────────────

void modeIndicator(AtlasContext& ctx, Vec2 pos,
                   const char* modeText, const Color& color) {
    if (!modeText || modeText[0] == '\0') return;

    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    float tw = rr.measureText(modeText);
    float padding = 12.0f;
    float w = tw + padding * 2.0f;
    float h = 26.0f;
    float x = pos.x - w * 0.5f;
    float y = pos.y;

    Color accentCol = (color.a > 0.01f) ? color : t.accentPrimary;

    // Dark background pill
    rr.drawRoundedRect({x, y, w, h}, t.bgPanel.withAlpha(0.85f), 4.0f);
    rr.drawRoundedRectOutline({x, y, w, h}, accentCol.withAlpha(0.6f), 4.0f);

    // Left accent bar
    rr.drawRect({x, y + 2.0f, 3.0f, h - 4.0f}, accentCol);

    // Text
    float textY = y + (h - 13.0f) * 0.5f;
    rr.drawText(modeText, {x + padding, textY}, accentCol);
}

// ── Damage Flash Overlay ────────────────────────────────────────────

void damageFlashOverlay(AtlasContext& ctx, Vec2 centre, float radius,
                        int layer, float intensity) {
    if (intensity <= 0.0f) return;

    const Theme& t = ctx.theme();
    Color flashColor;
    switch (layer) {
        case 0: flashColor = t.shield;  break;  // blue
        case 1: flashColor = t.armor;   break;  // gold
        case 2: flashColor = t.hull;    break;  // red
        default: flashColor = t.shield; break;
    }

    float alpha = intensity * 0.35f;

    // Draw concentric rings that fade outward
    int rings = 3;
    for (int i = 0; i < rings; ++i) {
        float innerR = radius + i * 15.0f;
        float outerR = innerR + 12.0f;
        float ringAlpha = alpha * (1.0f - static_cast<float>(i) / rings);
        Color ringColor = flashColor.withAlpha(ringAlpha);

        ctx.renderer().drawArc(centre, innerR, outerR,
                               0.0f, 2.0f * static_cast<float>(M_PI),
                               ringColor, 32);
    }
}

} // namespace atlas
