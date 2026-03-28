#include "noise.h"
#include <cmath>

NoiseNode::NoiseNode() {
    color = {30, 200, 120};
    color2 = {10, 20, 80};
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addFloat("scale", "Scale", 20.0f, 1.0f, 100.0f, 1.0f);
    params.addFloat("speed", "Speed", 2.0f, 0.0f, 10.0f, 0.1f);
    params.addInt("octaves", "Octaves", 2, 1, 4);
    params.addColor("color", "Color 1", color.r, color.g, color.b);
    params.addColor("color2", "Color 2", color2.r, color2.g, color2.b);
}

void NoiseNode::applyParams() {
    scale = params.get("scale");
    speed = params.get("speed");
    octaves = (int)params.get("octaves");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
    color2.r = (uint8_t)params.get("color2_r");
    color2.g = (uint8_t)params.get("color2_g");
    color2.b = (uint8_t)params.get("color2_b");
}

void NoiseNode::syncParams() {
    params.set("scale", scale);
    params.set("speed", speed);
    params.set("octaves", (float)octaves);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
    params.set("color2_r", (float)color2.r);
    params.set("color2_g", (float)color2.g);
    params.set("color2_b", (float)color2.b);
}

void NoiseNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_time += dt * speed;
}

// Hash-based pseudo-random noise
float NoiseNode::hash(float x, float y) {
    int ix = (int)floorf(x) * 1619;
    int iy = (int)floorf(y) * 31337;
    int n = ix + iy + 1013;
    n = (n << 13) ^ n;
    return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

float NoiseNode::smoothNoise(float x, float y) {
    float corners = (hash(x-1,y-1) + hash(x+1,y-1) + hash(x-1,y+1) + hash(x+1,y+1)) / 16.0f;
    float sides   = (hash(x-1,y) + hash(x+1,y) + hash(x,y-1) + hash(x,y+1)) / 8.0f;
    float center  = hash(x,y) / 4.0f;
    return corners + sides + center;
}

float NoiseNode::interpolatedNoise(float x, float y) {
    int ix = (int)floorf(x);
    int iy = (int)floorf(y);
    float fx = x - ix;
    float fy = y - iy;

    // Smooth interpolation
    fx = fx * fx * (3.0f - 2.0f * fx);
    fy = fy * fy * (3.0f - 2.0f * fy);

    float v1 = smoothNoise((float)ix, (float)iy);
    float v2 = smoothNoise((float)(ix+1), (float)iy);
    float v3 = smoothNoise((float)ix, (float)(iy+1));
    float v4 = smoothNoise((float)(ix+1), (float)(iy+1));

    float i1 = v1 + (v2 - v1) * fx;
    float i2 = v3 + (v4 - v3) * fx;
    return i1 + (i2 - i1) * fy;
}

float NoiseNode::fractalNoise(float x, float y, int oct) const {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxVal = 0.0f;

    for (int i = 0; i < oct; i++) {
        total += interpolatedNoise(x * frequency, y * frequency) * amplitude;
        maxVal += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    return total / maxVal;
}

void NoiseNode::render(Renderer& r) {
    if (!active) return;

    // Use 4x4 blocks for performance on ARM
    const int BLOCK = 4;
    float invScale = 1.0f / scale;

    for (int by = 0; by < h; by += BLOCK) {
        for (int bx = 0; bx < w; bx += BLOCK) {
            float nx = (bx + x) * invScale + m_time;
            float ny = (by + y) * invScale + m_time * 0.7f;

            float n = fractalNoise(nx, ny, octaves);
            float t = n * 0.5f + 0.5f; // normalize to 0-1
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            Color c;
            c.r = (uint8_t)(color.r + (color2.r - color.r) * t);
            c.g = (uint8_t)(color.g + (color2.g - color.g) * t);
            c.b = (uint8_t)(color.b + (color2.b - color.b) * t);

            // Fill block
            int maxX = std::min(bx + BLOCK, w);
            int maxY = std::min(by + BLOCK, h);
            for (int py = by; py < maxY; py++) {
                for (int px = bx; px < maxX; px++) {
                    r.pixel(x + px, y + py, c);
                }
            }
        }
    }
}
