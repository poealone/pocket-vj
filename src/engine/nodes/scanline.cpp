#include "scanline.h"

ScanlineNode::ScanlineNode() {
    params.addInt("spacing", "Spacing", 2, 1, 8);
    params.addInt("thickness", "Thickness", 1, 1, 4);
    params.addFloat("intensity", "Intensity", 0.5f, 0.0f, 1.0f, 0.05f);
}

void ScanlineNode::applyParams() {
    spacing = (int)params.get("spacing");
    thickness = (int)params.get("thickness");
    scanIntensity = params.get("intensity");
}

void ScanlineNode::syncParams() {
    params.set("spacing", (float)spacing);
    params.set("thickness", (float)thickness);
    params.set("intensity", scanIntensity);
}

void ScanlineNode::update(float /*dt*/, float /*audioLevel*/) {}

void ScanlineNode::render(Renderer& r) {
    if (!active || scanIntensity <= 0.0f) return;

    uint32_t* px = r.pixels();
    float darken = 1.0f - scanIntensity;

    for (int row = 0; row < RENDER_H; row++) {
        // Determine if this row is a "dark" scanline row
        int inCycle = row % (spacing + thickness);
        if (inCycle >= spacing) {
            // This is a scanline row — darken it
            for (int col = 0; col < RENDER_W; col++) {
                uint32_t p = px[row * RENDER_W + col];
                uint8_t pr = (uint8_t)(((p >> 16) & 0xFF) * darken);
                uint8_t pg = (uint8_t)(((p >> 8) & 0xFF) * darken);
                uint8_t pb = (uint8_t)((p & 0xFF) * darken);
                px[row * RENDER_W + col] = 0xFF000000 | (pr << 16) | (pg << 8) | pb;
            }
        }
    }
}
