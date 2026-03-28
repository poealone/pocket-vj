#pragma once
#include "../visual_node.h"

class GridNode : public VisualNode {
public:
    GridNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "GRID"; }
    const char* description() const override { return "Animated dot grid"; }

    void applyParams() override;
    void syncParams() override;

    int   rows = 12;
    int   cols = 16;
    int   spacing = 16;
    int   dotSize = 2;
    float waveAmount = 8.0f;
    float waveSpeed = 3.0f;

private:
    float m_time = 0.0f;
};
