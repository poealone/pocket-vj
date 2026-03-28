#pragma once
#include "../visual_node.h"

class BlurNode : public VisualNode {
public:
    BlurNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "BLUR"; }
    const char* description() const override { return "Box blur effect"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    int radius = 2;
    int passes = 1;

private:
    uint32_t m_temp[RENDER_W * RENDER_H];
};
