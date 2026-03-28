#include "shapes.h"
#include <cmath>
#include <algorithm>

ShapesNode::ShapesNode() {
    color = Palette::MAGENTA;
    x = RENDER_W / 2 - 30;
    y = RENDER_H / 2 - 30;
    w = 60;
    h = 60;
}

void ShapesNode::update(float dt, float audioLevel) {
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
            // 3 points, 120° apart
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

void ShapesNode::setParam(const std::string& name, float value) {
    if (name == "shape") shape = (ShapeType)(int)value;
    else if (name == "filled") filled = (value > 0.5f);
    else if (name == "rot_speed") rotSpeed = value;
    else if (name == "pulse_speed") pulseSpeed = value;
    else if (name == "intensity") intensity = value;
    else if (name == "color_r") color.r = (uint8_t)value;
    else if (name == "color_g") color.g = (uint8_t)value;
    else if (name == "color_b") color.b = (uint8_t)value;
}
