#include "widgets.h"
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace Widgets {

void drawSlider(Renderer& r, int x, int y, int w, const Param& p, bool selected) {
    // Label (8 chars max)
    std::string label = p.displayName;
    if (label.size() > 8) label = label.substr(0, 8);

    Color labelColor = selected ? Palette::RED : Palette::UI_FG;
    r.text(x, y, label, labelColor);

    // Slider track
    int sliderX = x + 50;  // After label
    int sliderW = w - 90;  // Leave room for value text
    if (sliderW < 20) sliderW = 20;

    // Background track
    r.rect(sliderX, y, sliderW, 5, {40, 40, 50}, true);

    // Filled portion
    float norm = p.normalized();
    int fillW = (int)(norm * sliderW);
    Color fillColor = selected ? Palette::RED : Palette::CYAN;
    if (fillW > 0) {
        r.rect(sliderX, y, fillW, 5, fillColor, true);
    }

    // Cursor marker
    int cursorX = sliderX + fillW;
    r.rect(cursorX - 1, y - 1, 3, 7, Palette::WHITE, true);

    // Value text
    char buf[16];
    if (p.type == ParamType::INT) {
        snprintf(buf, sizeof(buf), "%d", (int)p.value);
    } else {
        if (std::abs(p.value) < 10.0f) {
            snprintf(buf, sizeof(buf), "%.2f", p.value);
        } else {
            snprintf(buf, sizeof(buf), "%.1f", p.value);
        }
    }
    r.text(sliderX + sliderW + 4, y, buf, Palette::UI_FG);
}

void drawKnob(Renderer& r, int x, int y, const Param& p, bool selected) {
    std::string label = p.displayName;
    if (label.size() > 8) label = label.substr(0, 8);

    Color labelColor = selected ? Palette::RED : Palette::UI_FG;
    r.text(x, y, label, labelColor);

    // Draw a simple circular indicator
    int knobX = x + 60;
    int knobY = y + 2;
    int radius = 4;

    r.circle(knobX, knobY, radius, {40, 40, 50}, false);

    // Indicator line based on normalized value
    float norm = p.normalized();
    float angle = -2.4f + norm * 4.8f;  // ~270° sweep
    int ex = knobX + (int)(cosf(angle) * radius);
    int ey = knobY + (int)(sinf(angle) * radius);
    Color dotColor = selected ? Palette::RED : Palette::CYAN;
    r.line(knobX, knobY, ex, ey, dotColor);

    // Value
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f", p.value);
    r.text(knobX + radius + 4, y, buf, Palette::UI_FG);
}

void drawEnum(Renderer& r, int x, int y, const Param& p, bool selected) {
    std::string label = p.displayName;
    if (label.size() > 8) label = label.substr(0, 8);

    Color labelColor = selected ? Palette::RED : Palette::UI_FG;
    r.text(x, y, label, labelColor);

    // ◀ VALUE ▶
    int enumX = x + 50;
    std::string val = p.enumValue();

    Color arrowColor = selected ? Palette::RED : Palette::GRID;
    r.text(enumX, y, "<", arrowColor);
    r.text(enumX + 8, y, val, selected ? Palette::WHITE : Palette::CYAN);
    int endX = enumX + 8 + (int)val.size() * 5 + 2;
    r.text(endX, y, ">", arrowColor);
}

void drawToggle(Renderer& r, int x, int y, const Param& p, bool selected) {
    std::string label = p.displayName;
    if (label.size() > 8) label = label.substr(0, 8);

    Color labelColor = selected ? Palette::RED : Palette::UI_FG;
    r.text(x, y, label, labelColor);

    int toggleX = x + 50;
    bool on = p.boolValue();

    if (on) {
        r.rect(toggleX, y - 1, 20, 7, {30, 80, 30}, true);
        r.text(toggleX + 2, y, "ON", Palette::WHITE);
    } else {
        r.rect(toggleX, y - 1, 24, 7, {50, 30, 30}, true);
        r.text(toggleX + 2, y, "OFF", {150, 100, 100});
    }
}

void drawColorSwatch(Renderer& r, int x, int y, uint8_t cr, uint8_t cg, uint8_t cb) {
    r.rect(x, y, 8, 6, {cr, cg, cb}, true);
    r.rect(x, y, 8, 6, Palette::UI_FG, false);
}

void drawParam(Renderer& r, int x, int y, int w, const Param& p, bool selected) {
    switch (p.type) {
        case ParamType::FLOAT:
            drawSlider(r, x, y, w, p, selected);
            break;
        case ParamType::INT:
            drawSlider(r, x, y, w, p, selected);
            break;
        case ParamType::ENUM:
            drawEnum(r, x, y, p, selected);
            break;
        case ParamType::TOGGLE:
            drawToggle(r, x, y, p, selected);
            break;
        case ParamType::COLOR:
            // COLOR is decomposed into 3 INT params, so this shouldn't be called directly
            drawSlider(r, x, y, w, p, selected);
            break;
    }
}

} // namespace Widgets
