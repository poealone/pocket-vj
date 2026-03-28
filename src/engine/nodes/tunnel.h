#pragma once
#include "../visual_node.h"

class TunnelNode : public VisualNode {
public:
    TunnelNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "TUNL"; }
    const char* description() const override { return "Infinite tunnel effect"; }

    void applyParams() override;
    void syncParams() override;

    float speed = 3.0f;
    int   segments = 8;
    float rotation = 0.0f;
    Color color2 = Palette::BLUE;

private:
    float m_time = 0.0f;
};
