#include "ui/atlas/atlas_widgets.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static constexpr float METERS_PER_AU = 149597870700.0f;

namespace atlas {

// ── Checkbox ────────────────────────────────────────────────────────

bool checkbox(AtlasContext& ctx, const char* label,
              const Rect& r, bool* checked) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = ctx.currentID(label);

    float boxSz = std::min(r.h - 4.0f, 16.0f);
    Rect box = {r.x + 2.0f, r.y + (r.h - boxSz) * 0.5f, boxSz, boxSz};
    bool clicked = ctx.buttonBehavior(box, id);

    // Box background
    Color bg = t.bgSecondary;
    if (ctx.isHot(id)) bg = t.hover;
    rr.drawRect(box, bg);
    rr.drawRectOutline(box, t.borderNormal);

    // Check mark (simple filled inner square when checked)
    if (checked && *checked) {
        float inset = 3.0f;
        Rect inner = {box.x + inset, box.y + inset,
                      box.w - 2 * inset, box.h - 2 * inset};
        rr.drawRect(inner, t.accentPrimary);
    }

    // Label text
    float textX = box.right() + 6.0f;
    float textY = r.y + (r.h - 13.0f) * 0.5f;
    rr.drawText(label, {textX, textY}, t.textPrimary);

    if (clicked && checked) {
        *checked = !(*checked);
        return true;
    }
    return false;
}

// ── ComboBox ────────────────────────────────────────────────────────

bool comboBox(AtlasContext& ctx, const char* label,
              const Rect& r, const std::vector<std::string>& items,
              int* selected, bool* dropdownOpen) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = ctx.currentID(label);
    bool changed = false;

    // Main combo button
    bool mainClicked = ctx.buttonBehavior(r, id);

    Color bg = t.bgSecondary;
    if (ctx.isHot(id)) bg = t.hover;
    rr.drawRect(r, bg);
    rr.drawRectOutline(r, t.borderNormal);

    // Current selection text
    if (selected && *selected >= 0 && *selected < static_cast<int>(items.size())) {
        float textY = r.y + (r.h - 13.0f) * 0.5f;
        rr.drawText(items[*selected], {r.x + 6.0f, textY}, t.textPrimary);
    }

    // Dropdown arrow indicator
    float arrowX = r.right() - 14.0f;
    float arrowY = r.y + r.h * 0.5f;
    rr.drawLine({arrowX, arrowY - 3.0f}, {arrowX + 4.0f, arrowY + 3.0f},
                t.textSecondary, 1.0f);
    rr.drawLine({arrowX + 4.0f, arrowY + 3.0f}, {arrowX + 8.0f, arrowY - 3.0f},
                t.textSecondary, 1.0f);

    // Toggle dropdown on click
    if (mainClicked && dropdownOpen) {
        *dropdownOpen = !(*dropdownOpen);
    }

    // Dropdown list
    if (dropdownOpen && *dropdownOpen && !items.empty()) {
        float itemH = 22.0f;
        float dropH = itemH * items.size();
        Rect dropRect = {r.x, r.bottom(), r.w, dropH};

        rr.drawRect(dropRect, t.bgPanel);
        rr.drawRectOutline(dropRect, t.borderNormal);

        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            Rect itemRect = {r.x, r.bottom() + i * itemH, r.w, itemH};
            WidgetID itemID = ctx.currentID(items[i].c_str());

            bool itemClicked = ctx.buttonBehavior(itemRect, itemID);

            Color itemBg = (i % 2 == 0) ? t.bgSecondary.withAlpha(0.3f)
                                         : t.bgPanel.withAlpha(0.3f);
            if (selected && i == *selected) itemBg = t.selection;
            else if (ctx.isHot(itemID)) itemBg = t.hover;

            rr.drawRect(itemRect, itemBg);
            float textY = itemRect.y + (itemH - 13.0f) * 0.5f;
            rr.drawText(items[i], {itemRect.x + 6.0f, textY}, t.textPrimary);

            if (itemClicked && selected) {
                *selected = i;
                *dropdownOpen = false;
                changed = true;
            }
        }
    }

    return changed;
}

// ── Slider ──────────────────────────────────────────────────────────

bool slider(AtlasContext& ctx, const char* label,
            const Rect& r, float* value,
            float minVal, float maxVal,
            const char* format) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = ctx.currentID(label);
    bool changed = false;

    // Track background
    rr.drawRect(r, t.bgSecondary.withAlpha(0.5f));
    rr.drawRectOutline(r, t.borderSubtle);

    if (!value) return false;

    float range = maxVal - minVal;
    if (range <= 0.0f) return false;

    float frac = (*value - minVal) / range;
    frac = std::max(0.0f, std::min(1.0f, frac));

    // Filled portion
    if (frac > 0.0f) {
        rr.drawRect({r.x, r.y, r.w * frac, r.h}, t.accentDim.withAlpha(0.6f));
    }

    // Thumb (small vertical bar)
    float thumbX = r.x + r.w * frac;
    float thumbW = 6.0f;
    Rect thumbRect = {thumbX - thumbW * 0.5f, r.y, thumbW, r.h};

    bool hovered = ctx.isHovered(r);
    if (hovered) ctx.setHot(id);

    Color thumbColor = t.accentPrimary;
    if (ctx.isActive(id)) thumbColor = t.accentSecondary;
    else if (ctx.isHot(id)) thumbColor = t.accentPrimary.withAlpha(0.8f);

    rr.drawRect(thumbRect, thumbColor);

    // Interaction: click or drag to set value
    if (hovered && ctx.isMouseClicked()) {
        ctx.setActive(id);
    }

    if (ctx.isActive(id)) {
        if (ctx.isMouseDown()) {
            float mouseX = ctx.input().mousePos.x;
            float newFrac = (mouseX - r.x) / r.w;
            newFrac = std::max(0.0f, std::min(1.0f, newFrac));
            float newValue = minVal + newFrac * range;
            if (newValue != *value) {
                *value = newValue;
                changed = true;
            }
        } else {
            ctx.clearActive();
        }
    }

    // Value label
    if (format) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), format, *value);
        float tw = rr.measureText(buf);
        float tx = r.x + (r.w - tw) * 0.5f;
        float ty = r.y + (r.h - 13.0f) * 0.5f;
        rr.drawText(buf, {tx, ty}, t.textPrimary);
    }

    return changed;
}

// ── Text Input ──────────────────────────────────────────────────────

bool textInput(AtlasContext& ctx, const char* label,
               const Rect& r, TextInputState& state,
               const char* placeholder) {
    const Theme& t = ctx.theme();
    auto& rr = ctx.renderer();

    WidgetID id = ctx.currentID(label);
    bool changed = false;

    // Background
    Color bg = state.focused ? t.bgSecondary : t.bgSecondary.withAlpha(0.5f);
    rr.drawRect(r, bg);

    // Border
    Color borderCol = state.focused ? t.accentPrimary : t.borderNormal;
    rr.drawRectOutline(r, borderCol);

    // Click to focus
    bool hovered = ctx.isHovered(r);
    if (hovered) ctx.setHot(id);

    if (hovered && ctx.isMouseClicked()) {
        state.focused = true;
        ctx.setActive(id);
    } else if (ctx.isMouseClicked() && !hovered) {
        state.focused = false;
    }

    // Text display
    float textY = r.y + (r.h - 13.0f) * 0.5f;
    float textX = r.x + 6.0f;

    if (state.text.empty() && !state.focused && placeholder) {
        rr.drawText(placeholder, {textX, textY}, t.textDisabled);
    } else {
        rr.drawText(state.text, {textX, textY}, t.textPrimary);
    }

    // Blinking cursor (approximated — draw when focused)
    if (state.focused) {
        int pos = std::min(state.cursorPos, static_cast<int>(state.text.size()));
        std::string beforeCursor = state.text.substr(0, pos);
        float cursorX = textX + rr.measureText(beforeCursor);
        rr.drawRect({cursorX, r.y + 3.0f, 1.0f, r.h - 6.0f}, t.textPrimary);
    }

    return changed;
}

} // namespace atlas
