#include "starfield.h"
#include <cmath>

StarfieldNode::StarfieldNode() {
    color = Palette::WHITE;
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addInt("stars", "Star Count", 64, 10, 128);
    params.addFloat("speed", "Speed", 3.0f, 0.0f, 10.0f, 0.1f);
    params.addFloat("depth", "Depth", 50.0f, 1.0f, 100.0f, 1.0f);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void StarfieldNode::applyParams() {
    starCount = (int)params.get("stars");
    if (starCount > MAX_PARTICLES) starCount = MAX_PARTICLES;
    speed = params.get("speed");
    depth = params.get("depth");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void StarfieldNode::syncParams() {
    params.set("stars", (float)starCount);
    params.set("speed", speed);
    params.set("depth", depth);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

float StarfieldNode::fastRand() {
    m_seed = m_seed * 1103515245 + 12345;
    return ((float)(m_seed & 0x7fff)) / 32768.0f;
}

void StarfieldNode::initStar(Star& s, bool randomZ) {
    s.x = (fastRand() - 0.5f) * 2.0f * (float)w;
    s.y = (fastRand() - 0.5f) * 2.0f * (float)h;
    s.z = randomZ ? (fastRand() * depth) : depth;
}

void StarfieldNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    if (!m_initialized) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            initStar(m_stars[i], true);
        }
        m_initialized = true;
    }

    float moveZ = dt * speed * 20.0f;
    for (int i = 0; i < starCount; i++) {
        m_stars[i].z -= moveZ;
        if (m_stars[i].z <= 0.5f) {
            initStar(m_stars[i], false);
        }
    }
}

void StarfieldNode::render(Renderer& r) {
    if (!active) return;

    int cx = x + w / 2;
    int cy = y + h / 2;

    for (int i = 0; i < starCount; i++) {
        Star& s = m_stars[i];
        if (s.z <= 0.5f) continue;

        // 3D → 2D projection
        float px = s.x / s.z + cx;
        float py = s.y / s.z + cy;

        int sx = (int)px;
        int sy = (int)py;

        if (sx < x || sx >= x + w || sy < y || sy >= y + h) continue;

        // Brightness based on proximity
        float brightness = 1.0f - (s.z / depth);
        if (brightness < 0.0f) brightness = 0.0f;
        if (brightness > 1.0f) brightness = 1.0f;

        Color c;
        c.r = (uint8_t)(color.r * brightness);
        c.g = (uint8_t)(color.g * brightness);
        c.b = (uint8_t)(color.b * brightness);

        // Closer stars are larger
        if (brightness > 0.7f) {
            r.pixel(sx, sy, c);
            r.pixel(sx + 1, sy, c);
            r.pixel(sx, sy + 1, c);
            r.pixel(sx + 1, sy + 1, c);
        } else if (brightness > 0.3f) {
            r.pixel(sx, sy, c);
            r.pixel(sx + 1, sy, c);
        } else {
            r.pixel(sx, sy, c);
        }
    }
}
