#include "grid.h"
#include <cmath>

GridNode::GridNode() {
    color = Palette::CYAN;
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addInt("rows", "Rows", 12, 2, 32);
    params.addInt("cols", "Cols", 16, 2, 32);
    params.addInt("spacing", "Spacing", 16, 4, 32);
    params.addInt("dot_size", "Dot Size", 2, 1, 4);
    params.addFloat("wave_amt", "Wave Amt", 8.0f, 0.0f, 20.0f, 0.5f);
    params.addFloat("wave_spd", "Wave Spd", 3.0f, 0.0f, 10.0f, 0.1f);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void GridNode::applyParams() {
    rows = (int)params.get("rows");
    cols = (int)params.get("cols");
    spacing = (int)params.get("spacing");
    dotSize = (int)params.get("dot_size");
    waveAmount = params.get("wave_amt");
    waveSpeed = params.get("wave_spd");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void GridNode::syncParams() {
    params.set("rows", (float)rows);
    params.set("cols", (float)cols);
    params.set("spacing", (float)spacing);
    params.set("dot_size", (float)dotSize);
    params.set("wave_amt", waveAmount);
    params.set("wave_spd", waveSpeed);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

void GridNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_time += dt * waveSpeed;
}

void GridNode::render(Renderer& r) {
    if (!active) return;

    int offsetX = (w - (cols - 1) * spacing) / 2;
    int offsetY = (h - (rows - 1) * spacing) / 2;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            float baseX = (float)(x + offsetX + col * spacing);
            float baseY = (float)(y + offsetY + row * spacing);

            // Sine wave displacement
            float dist = sqrtf((float)(col * col + row * row));
            float dx = sinf(m_time + dist * 0.5f) * waveAmount;
            float dy = cosf(m_time * 0.8f + dist * 0.4f) * waveAmount;

            int px = (int)(baseX + dx);
            int py = (int)(baseY + dy);

            // Draw dot
            if (dotSize <= 1) {
                r.pixel(px, py, color);
            } else {
                r.circle(px, py, dotSize, color, true);
            }
        }
    }
}
