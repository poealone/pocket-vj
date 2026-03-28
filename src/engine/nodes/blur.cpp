#include "blur.h"
#include <cstring>
#include <algorithm>

BlurNode::BlurNode() {
    params.addInt("radius", "Radius", 2, 1, 8);
    params.addInt("passes", "Passes", 1, 1, 3);
}

void BlurNode::applyParams() {
    radius = (int)params.get("radius");
    passes = (int)params.get("passes");
}

void BlurNode::syncParams() {
    params.set("radius", (float)radius);
    params.set("passes", (float)passes);
}

void BlurNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

void BlurNode::render(Renderer& r) {
    if (!active || radius < 1) return;

    uint32_t* px = r.pixels();
    int rad = radius;

    for (int pass = 0; pass < passes; pass++) {
        // Horizontal pass: px -> m_temp
        if (rad > 4) {
            // Block-based for large radius
            int blockSize = 4;
            for (int row = 0; row < RENDER_H; row++) {
                for (int bx = 0; bx < RENDER_W; bx += blockSize) {
                    uint32_t sumR = 0, sumG = 0, sumB = 0;
                    int count = 0;
                    int x0 = std::max(0, bx - rad);
                    int x1 = std::min(RENDER_W - 1, bx + blockSize - 1 + rad);
                    for (int sx = x0; sx <= x1; sx++) {
                        uint32_t p = px[row * RENDER_W + sx];
                        sumR += (p >> 16) & 0xFF;
                        sumG += (p >> 8) & 0xFF;
                        sumB += p & 0xFF;
                        count++;
                    }
                    uint32_t avg = 0xFF000000 |
                        ((sumR / count) << 16) |
                        ((sumG / count) << 8) |
                        (sumB / count);
                    int endX = std::min(bx + blockSize, RENDER_W);
                    for (int fx = bx; fx < endX; fx++) {
                        m_temp[row * RENDER_W + fx] = avg;
                    }
                }
            }
        } else {
            for (int row = 0; row < RENDER_H; row++) {
                for (int col = 0; col < RENDER_W; col++) {
                    uint32_t sumR = 0, sumG = 0, sumB = 0;
                    int count = 0;
                    int x0 = std::max(0, col - rad);
                    int x1 = std::min(RENDER_W - 1, col + rad);
                    for (int sx = x0; sx <= x1; sx++) {
                        uint32_t p = px[row * RENDER_W + sx];
                        sumR += (p >> 16) & 0xFF;
                        sumG += (p >> 8) & 0xFF;
                        sumB += p & 0xFF;
                        count++;
                    }
                    m_temp[row * RENDER_W + col] = 0xFF000000 |
                        ((sumR / count) << 16) |
                        ((sumG / count) << 8) |
                        (sumB / count);
                }
            }
        }

        // Vertical pass: m_temp -> px
        for (int col = 0; col < RENDER_W; col++) {
            for (int row = 0; row < RENDER_H; row++) {
                uint32_t sumR = 0, sumG = 0, sumB = 0;
                int count = 0;
                int y0 = std::max(0, row - rad);
                int y1 = std::min(RENDER_H - 1, row + rad);
                for (int sy = y0; sy <= y1; sy++) {
                    uint32_t p = m_temp[sy * RENDER_W + col];
                    sumR += (p >> 16) & 0xFF;
                    sumG += (p >> 8) & 0xFF;
                    sumB += p & 0xFF;
                    count++;
                }
                px[row * RENDER_W + col] = 0xFF000000 |
                    ((sumR / count) << 16) |
                    ((sumG / count) << 8) |
                    (sumB / count);
            }
        }
    }
}
