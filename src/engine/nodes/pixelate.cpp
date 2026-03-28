#include "pixelate.h"
#include <algorithm>

PixelateNode::PixelateNode() {
    params.addInt("block_size", "Block Size", 4, 2, 16);
}

void PixelateNode::applyParams() {
    block_size = (int)params.get("block_size");
}

void PixelateNode::syncParams() {
    params.set("block_size", (float)block_size);
}

void PixelateNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

void PixelateNode::render(Renderer& r) {
    if (!active || block_size < 2) return;

    uint32_t* px = r.pixels();
    int bs = block_size;

    for (int by = 0; by < RENDER_H; by += bs) {
        for (int bx = 0; bx < RENDER_W; bx += bs) {
            // Average color within block
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            int count = 0;
            int endY = std::min(by + bs, RENDER_H);
            int endX = std::min(bx + bs, RENDER_W);

            for (int iy = by; iy < endY; iy++) {
                for (int ix = bx; ix < endX; ix++) {
                    uint32_t p = px[iy * RENDER_W + ix];
                    sumR += (p >> 16) & 0xFF;
                    sumG += (p >> 8) & 0xFF;
                    sumB += p & 0xFF;
                    count++;
                }
            }

            uint32_t avg = 0xFF000000 |
                ((sumR / count) << 16) |
                ((sumG / count) << 8) |
                (sumB / count);

            // Fill block with average
            for (int iy = by; iy < endY; iy++) {
                for (int ix = bx; ix < endX; ix++) {
                    px[iy * RENDER_W + ix] = avg;
                }
            }
        }
    }
}
