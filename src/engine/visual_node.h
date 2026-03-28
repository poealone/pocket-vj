#pragma once
#include "renderer.h"
#include "../config.h"
#include <string>

// Base class for all visual nodes
class VisualNode {
public:
    virtual ~VisualNode() = default;

    virtual void update(float dt, float audioLevel = 0.0f) = 0;
    virtual void render(Renderer& r) = 0;
    virtual const char* typeName() const = 0;

    // Common properties
    int x = 0, y = 0, w = RENDER_W, h = RENDER_H;
    Color color = Palette::RED;
    bool active = true;
    bool reactive = false;   // Audio-reactive
    float intensity = 1.0f;

    // Parameter access (for sequencer automation)
    virtual void setParam(const std::string& name, float value) {}
    virtual float getParam(const std::string& name) const { return 0.0f; }
};
