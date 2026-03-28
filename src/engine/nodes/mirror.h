#pragma once
#include "../visual_node.h"

enum class MirrorAxis { X, Y, BOTH };

class MirrorNode : public VisualNode {
public:
    MirrorNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "MIRR"; }
    const char* description() const override { return "Mirror framebuffer"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    MirrorAxis axis = MirrorAxis::X;
    float offset = 0.5f;
};
