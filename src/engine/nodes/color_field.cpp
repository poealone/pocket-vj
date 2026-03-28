#include "color_field.h"
#include <cmath>

ColorFieldNode::ColorFieldNode() {
    color = {18, 18, 40};
    color2 = {40, 10, 60};
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addEnum("type", "Type", {"SOLID", "GRAD V", "GRAD H", "PULSE"}, 1);
    params.addFloat("pulse_speed", "Pulse Spd", 1.0f, 0.1f, 10.0f, 0.1f);
    params.addColor("color", "Color 1", color.r, color.g, color.b);
    params.addColor("color2", "Color 2", color2.r, color2.g, color2.b);
}

void ColorFieldNode::applyParams() {
    fieldType = (FieldType)(int)params.get("type");
    pulseSpeed = params.get("pulse_speed");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
    color2.r = (uint8_t)params.get("color2_r");
    color2.g = (uint8_t)params.get("color2_g");
    color2.b = (uint8_t)params.get("color2_b");
}

void ColorFieldNode::syncParams() {
    params.set("type", (float)fieldType);
    params.set("pulse_speed", pulseSpeed);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
    params.set("color2_r", (float)color2.r);
    params.set("color2_g", (float)color2.g);
    params.set("color2_b", (float)color2.b);
}

void ColorFieldNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_phase += dt * pulseSpeed;
}

void ColorFieldNode::render(Renderer& r) {
    if (!active) return;

    switch (fieldType) {
        case FieldType::SOLID:
            r.rect(x, y, w, h, color, true);
            break;

        case FieldType::GRADIENT_V:
            for (int iy = 0; iy < h; iy++) {
                float t = (float)iy / h;
                Color c;
                c.r = (uint8_t)(color.r + (color2.r - color.r) * t);
                c.g = (uint8_t)(color.g + (color2.g - color.g) * t);
                c.b = (uint8_t)(color.b + (color2.b - color.b) * t);
                for (int ix = 0; ix < w; ix++) {
                    r.pixel(x + ix, y + iy, c);
                }
            }
            break;

        case FieldType::GRADIENT_H:
            for (int ix = 0; ix < w; ix++) {
                float t = (float)ix / w;
                Color c;
                c.r = (uint8_t)(color.r + (color2.r - color.r) * t);
                c.g = (uint8_t)(color.g + (color2.g - color.g) * t);
                c.b = (uint8_t)(color.b + (color2.b - color.b) * t);
                for (int iy = 0; iy < h; iy++) {
                    r.pixel(x + ix, y + iy, c);
                }
            }
            break;

        case FieldType::PULSE: {
            float t = sinf(m_phase) * 0.5f + 0.5f;
            Color c;
            c.r = (uint8_t)(color.r + (color2.r - color.r) * t);
            c.g = (uint8_t)(color.g + (color2.g - color.g) * t);
            c.b = (uint8_t)(color.b + (color2.b - color.b) * t);
            r.rect(x, y, w, h, c, true);
            break;
        }
    }
}
