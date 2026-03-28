#include "plasma.h"
#include <cmath>

PlasmaNode::PlasmaNode() {
    color = Palette::RED;
    color2 = {30, 60, 255};
    color3 = {255, 220, 30};
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addFloat("speed", "Speed", 3.0f, 0.0f, 10.0f, 0.1f);
    params.addFloat("scale", "Scale", 15.0f, 1.0f, 50.0f, 1.0f);
    params.addInt("complexity", "Complexity", 3, 1, 4);
    params.addColor("color", "Color 1", color.r, color.g, color.b);
    params.addColor("color2", "Color 2", color2.r, color2.g, color2.b);
    params.addColor("color3", "Color 3", color3.r, color3.g, color3.b);
}

void PlasmaNode::applyParams() {
    speed = params.get("speed");
    scale = params.get("scale");
    complexity = (int)params.get("complexity");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
    color2.r = (uint8_t)params.get("color2_r");
    color2.g = (uint8_t)params.get("color2_g");
    color2.b = (uint8_t)params.get("color2_b");
    color3.r = (uint8_t)params.get("color3_r");
    color3.g = (uint8_t)params.get("color3_g");
    color3.b = (uint8_t)params.get("color3_b");
}

void PlasmaNode::syncParams() {
    params.set("speed", speed);
    params.set("scale", scale);
    params.set("complexity", (float)complexity);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
    params.set("color2_r", (float)color2.r);
    params.set("color2_g", (float)color2.g);
    params.set("color2_b", (float)color2.b);
    params.set("color3_r", (float)color3.r);
    params.set("color3_g", (float)color3.g);
    params.set("color3_b", (float)color3.b);
}

void PlasmaNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_time += dt * speed;
}

void PlasmaNode::render(Renderer& r) {
    if (!active) return;

    // Use 4x4 blocks for ARM performance
    const int BLOCK = 4;
    float invScale = 1.0f / scale;

    for (int by = 0; by < h; by += BLOCK) {
        for (int bx = 0; bx < w; bx += BLOCK) {
            float fx = (float)(bx + x) * invScale;
            float fy = (float)(by + y) * invScale;

            // Layered sine waves
            float v = sinf(fx + m_time);
            if (complexity >= 2)
                v += sinf(fy * 1.3f + m_time * 0.7f);
            if (complexity >= 3)
                v += sinf((fx + fy) * 0.7f + m_time * 1.3f);
            if (complexity >= 4)
                v += sinf(sqrtf(fx * fx + fy * fy) + m_time * 0.5f);

            v = v / complexity; // normalize roughly to -1..1
            v = v * 0.5f + 0.5f; // to 0..1
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;

            // 3-color blend: color1 → color2 → color3
            Color c;
            if (v < 0.5f) {
                float t = v * 2.0f;
                c.r = (uint8_t)(color.r + (color2.r - color.r) * t);
                c.g = (uint8_t)(color.g + (color2.g - color.g) * t);
                c.b = (uint8_t)(color.b + (color2.b - color.b) * t);
            } else {
                float t = (v - 0.5f) * 2.0f;
                c.r = (uint8_t)(color2.r + (color3.r - color2.r) * t);
                c.g = (uint8_t)(color2.g + (color3.g - color2.g) * t);
                c.b = (uint8_t)(color2.b + (color3.b - color2.b) * t);
            }

            // Fill block
            int maxX = std::min(bx + BLOCK, w);
            int maxY = std::min(by + BLOCK, h);
            for (int py = by; py < maxY; py++) {
                for (int px = bx; px < maxX; px++) {
                    r.pixel(x + px, y + py, c);
                }
            }
        }
    }
}
