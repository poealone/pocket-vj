#include "param.h"

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
