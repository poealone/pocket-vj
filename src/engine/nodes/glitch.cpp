#include "glitch.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

GlitchNode::GlitchNode() {
    params.addFloat("intensity", "Intensity", 0.5f, 0.0f, 1.0f, 0.05f);
    params.addInt("block_size", "Block Size", 8, 2, 32);
    params.addInt("color_shift", "Color Shift", 5, 0, 20);
}

void GlitchNode::applyParams() {
    glitchIntensity = params.get("intensity");
    block_size = (int)params.get("block_size");
    color_shift = (int)params.get("color_shift");
}

void GlitchNode::syncParams() {
    params.set("intensity", glitchIntensity);
    params.set("block_size", (float)block_size);
    params.set("color_shift", (float)color_shift);
}

void GlitchNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

void GlitchNode::render(Renderer& r) {
    if (!active || glitchIntensity <= 0.0f) return;

    uint32_t* px = r.pixels();
    int bs = block_size;
    int cs = color_shift;

    // Randomly shift horizontal blocks
    for (int row = 0; row < RENDER_H; row += bs) {
        float chance = (float)rand() / RAND_MAX;
        if (chance > glitchIntensity) continue;

        int shift = (rand() % (RENDER_W / 2)) - RENDER_W / 4;
        shift = (int)(shift * glitchIntensity);
        int blockH = std::min(bs, RENDER_H - row);

        for (int by = 0; by < blockH; by++) {
            int srcRow = row + by;
            // Shift the line by copying pixels
            uint32_t lineBuf[RENDER_W];
            memcpy(lineBuf, &px[srcRow * RENDER_W], RENDER_W * sizeof(uint32_t));

            for (int col = 0; col < RENDER_W; col++) {
                int srcCol = col - shift;
                if (srcCol >= 0 && srcCol < RENDER_W) {
                    px[srcRow * RENDER_W + col] = lineBuf[srcCol];
                }
            }
        }
    }

    // RGB channel offset
    if (cs > 0) {
        for (int row = 0; row < RENDER_H; row++) {
            for (int col = 0; col < RENDER_W; col++) {
                uint32_t cur = px[row * RENDER_W + col];

                // Shift red channel right
                int redSrc = col + cs;
                uint8_t newR = (cur >> 16) & 0xFF;
                if (redSrc >= 0 && redSrc < RENDER_W) {
                    newR = (px[row * RENDER_W + redSrc] >> 16) & 0xFF;
                }

                // Shift blue channel left
                int blueSrc = col - cs;
                uint8_t newB = cur & 0xFF;
                if (blueSrc >= 0 && blueSrc < RENDER_W) {
                    newB = px[row * RENDER_W + blueSrc] & 0xFF;
                }

                uint8_t g = (cur >> 8) & 0xFF;
                px[row * RENDER_W + col] = 0xFF000000 | (newR << 16) | (g << 8) | newB;
            }
        }
    }
}
