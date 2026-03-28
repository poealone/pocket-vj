#pragma once
#include "../visual_node.h"

class FeedbackNode : public VisualNode {
public:
    FeedbackNode();
    ~FeedbackNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "FDBK"; }
    const char* description() const override { return "Frame feedback / trails"; }
    NodeCategory category() const override { return NodeCategory::FX; }

    void applyParams() override;
    void syncParams() override;

    float decay = 0.9f;
    int offset_x = 0;
    int offset_y = 0;
    float zoom = 1.0f;

private:
    uint32_t* m_buffer = nullptr;
    bool m_initialized = false;
};
