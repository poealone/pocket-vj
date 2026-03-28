#include "edge.h"
#include <cmath>
#include <algorithm>
#include <cstring>

EdgeNode::EdgeNode() {
    params.addInt("threshold", "Threshold", 64, 0, 255, 4);
    params.addColor("edge_color", "Edge Color", 255, 255, 255);
}

void EdgeNode::applyParams() {
    threshold = (int)params.get("threshold");
    edge_r = (uint8_t)params.get("edge_color_r");
    edge_g = (uint8_t)params.get("edge_color_g");
    edge_b = (uint8_t)params.get("edge_color_b");
}

void EdgeNode::syncParams() {
    params.set("threshold", (float)threshold);
    params.set("edge_color_r", (float)edge_r);
    params.set("edge_color_g", (float)edge_g);
    params.set("edge_color_b", (float)edge_b);
}

void EdgeNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

static inline uint8_t luminance(uint32_t p) {
    uint8_t r = (p >> 16) & 0xFF;
    uint8_t g = (p >> 8) & 0xFF;
    uint8_t b = p & 0xFF;
    return (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
}

void EdgeNode::render(Renderer& r) {
    if (!active) return;

    uint32_t* px = r.pixels();
    int thr = threshold;
    uint32_t edgeColor = 0xFF000000 | (edge_r << 16) | (edge_g << 8) | edge_b;

    // Sobel edge detection
    for (int row = 1; row < RENDER_H - 1; row++) {
        for (int col = 1; col < RENDER_W - 1; col++) {
            // 3x3 neighborhood luminance
            int tl = luminance(px[(row-1) * RENDER_W + (col-1)]);
            int tc = luminance(px[(row-1) * RENDER_W + col]);
            int tr = luminance(px[(row-1) * RENDER_W + (col+1)]);
            int ml = luminance(px[row * RENDER_W + (col-1)]);
            int mr = luminance(px[row * RENDER_W + (col+1)]);
            int bl = luminance(px[(row+1) * RENDER_W + (col-1)]);
            int bc = luminance(px[(row+1) * RENDER_W + col]);
            int br = luminance(px[(row+1) * RENDER_W + (col+1)]);

            // Sobel X: -1 0 +1 / -2 0 +2 / -1 0 +1
            int gx = -tl + tr - 2*ml + 2*mr - bl + br;
            // Sobel Y: -1 -2 -1 / 0 0 0 / +1 +2 +1
            int gy = -tl - 2*tc - tr + bl + 2*bc + br;

            int mag = (int)sqrtf((float)(gx*gx + gy*gy));
            m_temp[row * RENDER_W + col] = (mag > thr) ? edgeColor : 0xFF000000;
        }
    }

    // Border pixels = black
    for (int col = 0; col < RENDER_W; col++) {
        m_temp[col] = 0xFF000000;
        m_temp[(RENDER_H-1) * RENDER_W + col] = 0xFF000000;
    }
    for (int row = 0; row < RENDER_H; row++) {
        m_temp[row * RENDER_W] = 0xFF000000;
        m_temp[row * RENDER_W + RENDER_W - 1] = 0xFF000000;
    }

    // Copy result back
    memcpy(px, m_temp, RENDER_W * RENDER_H * sizeof(uint32_t));
}
