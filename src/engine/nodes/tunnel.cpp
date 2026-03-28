#include "tunnel.h"
#include <cmath>

TunnelNode::TunnelNode() {
    color = Palette::MAGENTA;
    color2 = {20, 10, 40};
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addFloat("speed", "Speed", 3.0f, 0.0f, 10.0f, 0.1f);
    params.addInt("segments", "Segments", 8, 4, 16);
    params.addFloat("rotation", "Rotation", 0.0f, 0.0f, 360.0f, 5.0f);
    params.addColor("color", "Color 1", color.r, color.g, color.b);
    params.addColor("color2", "Color 2", color2.r, color2.g, color2.b);
}

void TunnelNode::applyParams() {
    speed = params.get("speed");
    segments = (int)params.get("segments");
    rotation = params.get("rotation");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
    color2.r = (uint8_t)params.get("color2_r");
    color2.g = (uint8_t)params.get("color2_g");
    color2.b = (uint8_t)params.get("color2_b");
}

void TunnelNode::syncParams() {
    params.set("speed", speed);
    params.set("segments", (float)segments);
    params.set("rotation", rotation);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
    params.set("color2_r", (float)color2.r);
    params.set("color2_g", (float)color2.g);
    params.set("color2_b", (float)color2.b);
}

void TunnelNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_time += dt * speed;
}

void TunnelNode::render(Renderer& r) {
    if (!active) return;

    int cx = x + w / 2;
    int cy = y + h / 2;
    // Draw concentric rectangles zooming outward
    for (int i = segments - 1; i >= 0; i--) {
        // Phase offset creates the zoom animation
        float t = (float)i / segments;
        float phase = fmodf(m_time + t, 1.0f);
        float scale = phase; // 0 = center, 1 = edge

        int rw = (int)(scale * w);
        int rh = (int)(scale * h);

        if (rw < 2 || rh < 2) continue;

        // Alternate colors
        Color c;
        if (i % 2 == 0) {
            float blend = scale;
            c.r = (uint8_t)(color.r + (color2.r - color.r) * blend);
            c.g = (uint8_t)(color.g + (color2.g - color.g) * blend);
            c.b = (uint8_t)(color.b + (color2.b - color.b) * blend);
        } else {
            c = color2;
        }

        // Apply rotation offset
        float rotRad = rotation * 3.14159265f / 180.0f;
        int ox = (int)(sinf(rotRad + m_time * 0.5f) * scale * 10.0f);
        int oy = (int)(cosf(rotRad + m_time * 0.5f) * scale * 10.0f);

        r.rect(cx - rw/2 + ox, cy - rh/2 + oy, rw, rh, c, false);
    }
}
