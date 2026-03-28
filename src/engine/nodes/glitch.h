#pragma once
#include "../visual_node.h"

class GlitchNode : public VisualNode {
public:
    GlitchNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "GLTC"; }
    const char* description() const override { return "Digital glitch effect"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    float glitchIntensity = 0.5f;
    int block_size = 8;
    int color_shift = 5;
};
