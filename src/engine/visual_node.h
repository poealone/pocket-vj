#pragma once
#include "renderer.h"
#include "param.h"
#include "../config.h"
#include <string>

// Node category for browser
enum class NodeCategory { SOURCE, TRANSFORM, FX, MODULATOR, THREED };

// Base class for all visual nodes
class VisualNode {
public:
    virtual ~VisualNode() = default;

    virtual void update(float dt, float audioLevel = 0.0f) = 0;
    virtual void render(Renderer& r) = 0;
    virtual const char* typeName() const = 0;
    virtual const char* description() const { return ""; }
    virtual NodeCategory category() const { return NodeCategory::SOURCE; }

    // Common properties
    int x = 0, y = 0, w = RENDER_W, h = RENDER_H;
    Color color = Palette::RED;
    bool active = true;
    bool reactive = false;
    float intensity = 1.0f;

    // Parameter system
    ParamList params;

    // Sync node state FROM params (call after param changes)
    virtual void applyParams() {}

    // Sync params FROM node state (call to update param display values)
    virtual void syncParams() {}

    // Legacy interface — routes through ParamList
    virtual void setParam(const std::string& name, float value) {
        params.set(name, value);
        applyParams();
    }
    virtual float getParam(const std::string& name) const {
        return params.get(name);
    }
};
