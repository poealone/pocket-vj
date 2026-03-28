#include "param.h"
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Param::animate(float dt) {
    if (!animated) return;

    float oldPhase = animPhase;
    animPhase += animSpeed * dt;
    // Wrap to 0..1
    animPhase -= (float)(int)animPhase;
    if (animPhase < 0.0f) animPhase += 1.0f;

    float t = 0.0f;
    switch (animShape) {
        case 0: // SINE
            t = 0.5f + 0.5f * sinf(animPhase * 2.0f * (float)M_PI);
            break;
        case 1: // TRI
            t = animPhase < 0.5f ? animPhase * 2.0f : 2.0f - animPhase * 2.0f;
            break;
        case 2: // SAW
            t = animPhase;
            break;
        case 3: // SQUARE
            t = animPhase < 0.5f ? 1.0f : 0.0f;
            break;
        case 4: // RANDOM
            // New random value when phase wraps (goes from high to low)
            if (animPhase < oldPhase) {
                animRandVal = (float)rand() / (float)RAND_MAX;
            }
            t = animRandVal;
            break;
        default:
            t = 0.0f;
            break;
    }

    value = animMin + t * (animMax - animMin);
    clamp();
}

void ParamList::animateAll(float dt) {
    for (auto& p : m_params) {
        p.animate(dt);
    }
}

void ParamList::addFloat(const std::string& name, const std::string& display,
                         float def, float minV, float maxV, float stp) {
    Param p;
    p.name = name;
    p.displayName = display;
    p.type = ParamType::FLOAT;
    p.value = def;
    p.defValue = def;
    p.minVal = minV;
    p.maxVal = maxV;
    p.step = stp;
    m_params.push_back(p);
}

void ParamList::addInt(const std::string& name, const std::string& display,
                       int def, int minV, int maxV, int stp) {
    Param p;
    p.name = name;
    p.displayName = display;
    p.type = ParamType::INT;
    p.value = (float)def;
    p.defValue = (float)def;
    p.minVal = (float)minV;
    p.maxVal = (float)maxV;
    p.step = (float)stp;
    m_params.push_back(p);
}

void ParamList::addEnum(const std::string& name, const std::string& display,
                        const std::vector<std::string>& options, int defIndex) {
    Param p;
    p.name = name;
    p.displayName = display;
    p.type = ParamType::ENUM;
    p.value = (float)defIndex;
    p.defValue = (float)defIndex;
    p.minVal = 0.0f;
    p.maxVal = (float)(options.size() - 1);
    p.step = 1.0f;
    p.enumOptions = options;
    m_params.push_back(p);
}

void ParamList::addToggle(const std::string& name, const std::string& display, bool def) {
    Param p;
    p.name = name;
    p.displayName = display;
    p.type = ParamType::TOGGLE;
    p.value = def ? 1.0f : 0.0f;
    p.defValue = p.value;
    p.minVal = 0.0f;
    p.maxVal = 1.0f;
    p.step = 1.0f;
    m_params.push_back(p);
}

void ParamList::addColor(const std::string& name, const std::string& display,
                         uint8_t r, uint8_t g, uint8_t b) {
    // Store as 3 separate INT params: name_r, name_g, name_b
    addInt(name + "_r", display + " R", r, 0, 255, 5);
    addInt(name + "_g", display + " G", g, 0, 255, 5);
    addInt(name + "_b", display + " B", b, 0, 255, 5);
}

Param* ParamList::find(const std::string& name) {
    for (auto& p : m_params) {
        if (p.name == name) return &p;
    }
    return nullptr;
}

const Param* ParamList::find(const std::string& name) const {
    for (auto& p : m_params) {
        if (p.name == name) return &p;
    }
    return nullptr;
}

void ParamList::set(const std::string& name, float value) {
    Param* p = find(name);
    if (p) {
        p->value = value;
        p->clamp();
    }
}

float ParamList::get(const std::string& name) const {
    const Param* p = find(name);
    return p ? p->value : 0.0f;
}
