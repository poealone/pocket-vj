#include "colorshift.h"
#include <cmath>
#include <algorithm>

ColorShiftNode::ColorShiftNode() {
    params.addFloat("hue_shift", "Hue Shift", 0.0f, 0.0f, 360.0f, 5.0f);
    params.addFloat("saturation", "Saturation", 1.0f, 0.0f, 2.0f, 0.05f);
    params.addFloat("brightness", "Brightness", 1.0f, 0.0f, 2.0f, 0.05f);
}

void ColorShiftNode::applyParams() {
    hue_shift = params.get("hue_shift");
    saturation = params.get("saturation");
    brightness = params.get("brightness");
}

void ColorShiftNode::syncParams() {
    params.set("hue_shift", hue_shift);
    params.set("saturation", saturation);
    params.set("brightness", brightness);
}

void ColorShiftNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
}

void ColorShiftNode::rgbToHsv(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float maxC = std::max({rf, gf, bf});
    float minC = std::min({rf, gf, bf});
    float delta = maxC - minC;

    v = maxC;
    s = (maxC > 0.0f) ? delta / maxC : 0.0f;

    if (delta < 0.001f) {
        h = 0.0f;
    } else if (maxC == rf) {
        h = 60.0f * fmodf((gf - bf) / delta, 6.0f);
    } else if (maxC == gf) {
        h = 60.0f * ((bf - rf) / delta + 2.0f);
    } else {
        h = 60.0f * ((rf - gf) / delta + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
}

void ColorShiftNode::hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    s = std::max(0.0f, std::min(1.0f, s));
    v = std::max(0.0f, std::min(1.0f, v));

    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float rf, gf, bf;
    if (h < 60)       { rf = c; gf = x; bf = 0; }
    else if (h < 120) { rf = x; gf = c; bf = 0; }
    else if (h < 180) { rf = 0; gf = c; bf = x; }
    else if (h < 240) { rf = 0; gf = x; bf = c; }
    else if (h < 300) { rf = x; gf = 0; bf = c; }
    else              { rf = c; gf = 0; bf = x; }

    r = (uint8_t)((rf + m) * 255.0f);
    g = (uint8_t)((gf + m) * 255.0f);
    b = (uint8_t)((bf + m) * 255.0f);
}

void ColorShiftNode::render(Renderer& r) {
    if (!active) return;
    // Skip if no-op
    if (hue_shift == 0.0f && saturation == 1.0f && brightness == 1.0f) return;

    uint32_t* px = r.pixels();
    float hs = hue_shift;
    float sat = saturation;
    float brt = brightness;

    for (int i = 0; i < RENDER_W * RENDER_H; i++) {
        uint32_t p = px[i];
        uint8_t pr = (p >> 16) & 0xFF;
        uint8_t pg = (p >> 8) & 0xFF;
        uint8_t pb = p & 0xFF;

        // Skip black pixels (common optimization)
        if (pr == 0 && pg == 0 && pb == 0) continue;

        float h, s, v;
        rgbToHsv(pr, pg, pb, h, s, v);

        h = fmodf(h + hs, 360.0f);
        if (h < 0.0f) h += 360.0f;
        s *= sat;
        v *= brt;

        uint8_t nr, ng, nb;
        hsvToRgb(h, s, v, nr, ng, nb);

        px[i] = 0xFF000000 | (nr << 16) | (ng << 8) | nb;
    }
}
