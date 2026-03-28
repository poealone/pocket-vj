#pragma once
#include "../visual_node.h"

class EdgeNode : public VisualNode {
public:
    EdgeNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "EDGE"; }
    const char* description() const override { return "Sobel edge detection"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    int threshold = 64;
    uint8_t edge_r = 255, edge_g = 255, edge_b = 255;

private:
    uint32_t m_temp[RENDER_W * RENDER_H];
};
