#pragma once
#include "../visual_node.h"

class ScanlineNode : public VisualNode {
public:
    ScanlineNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "SCAN"; }
    const char* description() const override { return "CRT scanline overlay"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    int spacing = 2;
    int thickness = 1;
    float scanIntensity = 0.5f;
};
