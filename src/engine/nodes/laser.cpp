#include "laser.h"
#include <cmath>

LaserNode::LaserNode() {
    color = Palette::RED;
    x = RENDER_W / 2; y = RENDER_H / 2;
    w = RENDER_W; h = RENDER_H;

    params.addInt("points", "Points", 6, 3, 16);
    params.addFloat("speed", "Speed", 3.0f, 0.0f, 10.0f, 0.1f);
    params.addFloat("decay", "Decay", 0.5f, 0.0f, 1.0f, 0.05f);
    params.addInt("thickness", "Thickness", 2, 1, 4);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void LaserNode::applyParams() {
    points = (int)params.get("points");
    speed = params.get("speed");
    decay = params.get("decay");
    thickness = (int)params.get("thickness");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void LaserNode::syncParams() {
    params.set("points", (float)points);
    params.set("speed", speed);
    params.set("decay", decay);
    params.set("thickness", (float)thickness);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

void LaserNode::update(float dt, float audioLevel) {
    params.animateAll(dt);
    applyParams();
    m_angle += dt * speed;
    m_pulse += dt * 4.0f;
    if (reactive && audioLevel > 0.0f) {
        m_angle += dt * speed * audioLevel * 2.0f;
    }
}

void LaserNode::render(Renderer& r) {
    if (!active) return;

    int cx = x;
    int cy = y;
    float radius = std::min(w, h) * 0.45f;
    float pulseR = radius * (1.0f + sinf(m_pulse) * decay * 0.3f);

    float angleStep = 2.0f * 3.14159265f / points;

    for (int i = 0; i < points; i++) {
        float a = m_angle + i * angleStep;
        int px = cx + (int)(cosf(a) * pulseR);
        int py = cy + (int)(sinf(a) * pulseR);

        // Draw line from center to point
        for (int t = 0; t < thickness; t++) {
            r.line(cx + t, cy, px + t, py, color);
            r.line(cx, cy + t, px, py + t, color);
        }

        // Connect adjacent points
        float a2 = m_angle + ((i + 1) % points) * angleStep;
        int px2 = cx + (int)(cosf(a2) * pulseR);
        int py2 = cy + (int)(sinf(a2) * pulseR);
        for (int t = 0; t < thickness; t++) {
            r.line(px + t, py, px2 + t, py2, color);
        }
    }
}
