#pragma once
#include "../visual_node.h"

class ColorShiftNode : public VisualNode {
public:
    ColorShiftNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "CSHF"; }
    const char* description() const override { return "Hue/sat/brightness shift"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    float hue_shift = 0.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;

private:
    static void rgbToHsv(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v);
    static void hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);
};
