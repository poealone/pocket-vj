#pragma once
#include "../visual_node.h"

class PixelateNode : public VisualNode {
public:
    PixelateNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "PXLT"; }
    const char* description() const override { return "Mosaic / big pixel effect"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    int block_size = 4;
};
