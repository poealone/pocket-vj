#include "feedback.h"
#include <cstring>
#include <cmath>
#include <algorithm>

FeedbackNode::FeedbackNode() {
    params.addFloat("decay", "Decay", 0.9f, 0.01f, 1.0f, 0.01f);
    params.addInt("offset_x", "Offset X", 0, -10, 10);
    params.addInt("offset_y", "Offset Y", 0, -10, 10);
    params.addFloat("zoom", "Zoom", 1.0f, 0.9f, 1.1f, 0.01f);

    m_buffer = new uint32_t[RENDER_W * RENDER_H];
    memset(m_buffer, 0, RENDER_W * RENDER_H * sizeof(uint32_t));
}

FeedbackNode::~FeedbackNode() {
    delete[] m_buffer;
}

void FeedbackNode::applyParams() {
    decay = params.get("decay");
    offset_x = (int)params.get("offset_x");
    offset_y = (int)params.get("offset_y");
    zoom = params.get("zoom");
}

void FeedbackNode::syncParams() {
    params.set("decay", decay);
    params.set("offset_x", (float)offset_x);
    params.set("offset_y", (float)offset_y);
    params.set("zoom", zoom);
}

void FeedbackNode::update(float /*dt*/, float /*audioLevel*/) {}

void FeedbackNode::render(Renderer& r) {
    if (!active) return;

    uint32_t* px = r.pixels();
    float d = decay;
    float invD = 1.0f - d;
    float z = zoom;
    int ox = offset_x;
    int oy = offset_y;
    float cx = RENDER_W * 0.5f;
    float cy = RENDER_H * 0.5f;

    // Blend previous frame (from m_buffer) with current frame
    for (int row = 0; row < RENDER_H; row++) {
        for (int col = 0; col < RENDER_W; col++) {
            // Sample from feedback buffer with zoom + offset
            float sx = (col - cx) / z + cx + ox;
            float sy = (row - cy) / z + cy + oy;
            int ix = (int)sx;
            int iy = (int)sy;

            uint32_t fbPx = 0;
            if (ix >= 0 && ix < RENDER_W && iy >= 0 && iy < RENDER_H) {
                fbPx = m_buffer[iy * RENDER_W + ix];
            }

            uint32_t curPx = px[row * RENDER_W + col];

            // Blend: result = current * (1-decay) + feedback * decay
            uint8_t cr = (curPx >> 16) & 0xFF;
            uint8_t cg = (curPx >> 8) & 0xFF;
            uint8_t cb = curPx & 0xFF;

            uint8_t fr = (fbPx >> 16) & 0xFF;
            uint8_t fg = (fbPx >> 8) & 0xFF;
            uint8_t fb = fbPx & 0xFF;

            uint8_t rr = (uint8_t)std::min(255.0f, cr * invD + fr * d);
            uint8_t rg = (uint8_t)std::min(255.0f, cg * invD + fg * d);
            uint8_t rb = (uint8_t)std::min(255.0f, cb * invD + fb * d);

            uint32_t result = 0xFF000000 | (rr << 16) | (rg << 8) | rb;
            px[row * RENDER_W + col] = result;
        }
    }

    // Store current frame as feedback for next frame
    memcpy(m_buffer, px, RENDER_W * RENDER_H * sizeof(uint32_t));
}
