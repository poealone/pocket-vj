#include "color_field.h"
#include <cmath>

ColorFieldNode::ColorFieldNode() {
    color = {18, 18, 40};
    color2 = {40, 10, 60};
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;
}

void ColorFieldNode::update(float dt, float audioLevel) {
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

void ColorFieldNode::setParam(const std::string& name, float value) {
    if (name == "type") fieldType = (FieldType)(int)value;
    else if (name == "pulse_speed") pulseSpeed = value;
    else if (name == "color_r") color.r = (uint8_t)value;
    else if (name == "color_g") color.g = (uint8_t)value;
    else if (name == "color_b") color.b = (uint8_t)value;
    else if (name == "color2_r") color2.r = (uint8_t)value;
    else if (name == "color2_g") color2.g = (uint8_t)value;
    else if (name == "color2_b") color2.b = (uint8_t)value;
}
