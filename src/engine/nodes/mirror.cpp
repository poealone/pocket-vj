#include "mirror.h"
#include <cstring>

MirrorNode::MirrorNode() {
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addEnum("axis", "Axis", {"X", "Y", "BOTH"}, 0);
    params.addFloat("offset", "Offset", 0.5f, 0.0f, 1.0f, 0.05f);
}

void MirrorNode::applyParams() {
    axis = (MirrorAxis)(int)params.get("axis");
    offset = params.get("offset");
}

void MirrorNode::syncParams() {
    params.set("axis", (float)axis);
    params.set("offset", offset);
}

void MirrorNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

void MirrorNode::render(Renderer& r) {
    if (!active) return;

    uint32_t* px = r.pixels();
    int pitch = r.pitch();

    if (axis == MirrorAxis::X || axis == MirrorAxis::BOTH) {
        // Mirror left half to right
        int mid = (int)(RENDER_W * offset);
        for (int row = 0; row < RENDER_H; row++) {
            uint32_t* line = px + row * pitch;
            for (int col = 0; col < mid; col++) {
                int mirrorCol = mid + (mid - col - 1);
                if (mirrorCol >= 0 && mirrorCol < RENDER_W) {
                    line[mirrorCol] = line[col];
                }
            }
        }
    }

    if (axis == MirrorAxis::Y || axis == MirrorAxis::BOTH) {
        // Mirror top half to bottom
        int mid = (int)(RENDER_H * offset);
        for (int row = 0; row < mid; row++) {
            int mirrorRow = mid + (mid - row - 1);
            if (mirrorRow >= 0 && mirrorRow < RENDER_H) {
                memcpy(px + mirrorRow * pitch, px + row * pitch, RENDER_W * sizeof(uint32_t));
            }
        }
    }
}
