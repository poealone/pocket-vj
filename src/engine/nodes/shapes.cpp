#include "shapes.h"
#include <cmath>
#include <algorithm>

ShapesNode::ShapesNode() {
    color = Palette::MAGENTA;
    x = RENDER_W / 2 - 30;
    y = RENDER_H / 2 - 30;
    w = 60;
    h = 60;
    intensity = 1.0f;

    params.addEnum("shape", "Shape", {"RECT", "CIRCLE", "LINE", "TRI"}, 1);
    params.addToggle("filled", "Filled", true);
    params.addFloat("rot_speed", "Rot Speed", 1.0f, 0.0f, 10.0f, 0.5f);
    params.addFloat("pulse_speed", "Pulse Spd", 2.0f, 0.0f, 10.0f, 0.5f);
    params.addFloat("intensity", "Intensity", 1.0f, 0.0f, 2.0f, 0.1f);
    params.addToggle("reactive", "Reactive", false);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void ShapesNode::applyParams() {
    shape = (ShapeType)(int)params.get("shape");
    filled = params.get("filled") > 0.5f;
    rotSpeed = params.get("rot_speed");
    pulseSpeed = params.get("pulse_speed");
    intensity = params.get("intensity");
    reactive = params.get("reactive") > 0.5f;
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void ShapesNode::syncParams() {
    params.set("shape", (float)shape);
    params.set("filled", filled ? 1.0f : 0.0f);
    params.set("rot_speed", rotSpeed);
    params.set("pulse_speed", pulseSpeed);
    params.set("intensity", intensity);
    params.set("reactive", reactive ? 1.0f : 0.0f);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

void ShapesNode::update(float dt, float audioLevel) {
    params.animateAll(dt);
    applyParams();
    rotation += rotSpeed * dt;
    m_pulse += pulseSpeed * dt;

    if (reactive) {
        float scale = 1.0f + audioLevel * 0.5f;
        intensity = scale;
    }
}

void ShapesNode::render(Renderer& r) {
    if (!active) return;

    float pulse = sinf(m_pulse) * 0.2f + 0.8f;
    int pw = (int)(w * pulse * intensity);
    int ph = (int)(h * pulse * intensity);
    int cx = x + w / 2;
    int cy = y + h / 2;

    switch (shape) {
        case ShapeType::RECT:
            r.rect(cx - pw / 2, cy - ph / 2, pw, ph, color, filled);
            break;

        case ShapeType::CIRCLE:
            r.circle(cx, cy, std::min(pw, ph) / 2, color, filled);
            break;

        case ShapeType::LINE: {
            float rad = rotation;
            int len = std::min(pw, ph) / 2;
            int x1 = cx + (int)(cosf(rad) * len);
            int y1 = cy + (int)(sinf(rad) * len);
            int x2 = cx - (int)(cosf(rad) * len);
            int y2 = cy - (int)(sinf(rad) * len);
            r.line(x1, y1, x2, y2, color);
            break;
        }

        case ShapeType::TRIANGLE: {
            float rad = rotation;
            int len = std::min(pw, ph) / 2;
            for (int i = 0; i < 3; i++) {
                float a1 = rad + i * (6.28f / 3.0f);
                float a2 = rad + (i + 1) * (6.28f / 3.0f);
                int x1 = cx + (int)(cosf(a1) * len);
                int y1 = cy + (int)(sinf(a1) * len);
                int x2 = cx + (int)(cosf(a2) * len);
                int y2 = cy + (int)(sinf(a2) * len);
                r.line(x1, y1, x2, y2, color);
            }
            break;
        }
    }
}
