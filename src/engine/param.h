#pragma once
#include <string>
#include <vector>
#include <cstdint>

enum class ParamType {
    FLOAT,    // Continuous slider
    INT,      // Integer slider
    ENUM,     // Cycle through string options
    TOGGLE,   // On/off
    COLOR     // RGB triplet (3 sub-params)
};

struct Param {
    std::string name;          // Internal ID (e.g. "color_r")
    std::string displayName;   // UI label (e.g. "Color R")
    ParamType   type = ParamType::FLOAT;

    float value    = 0.0f;
    float defValue = 0.0f;
    float minVal   = 0.0f;
    float maxVal   = 1.0f;
    float step     = 0.1f;

    // ENUM options
    std::vector<std::string> enumOptions;

    // Helper: clamp value to range
    void clamp() {
        if (value < minVal) value = minVal;
        if (value > maxVal) value = maxVal;
    }

    // Adjust by delta, clamped
    void adjust(float delta) {
        value += delta;
        clamp();
    }

    // For ENUM: get current option string
    std::string enumValue() const {
        int idx = (int)value;
        if (idx >= 0 && idx < (int)enumOptions.size())
            return enumOptions[idx];
        return "???";
    }

    // For TOGGLE
    bool boolValue() const { return value > 0.5f; }
    void toggle() { value = boolValue() ? 0.0f : 1.0f; }

    // Normalized 0..1
    float normalized() const {
        if (maxVal <= minVal) return 0.0f;
        return (value - minVal) / (maxVal - minVal);
    }
};

// Ordered list of parameters for a node
class ParamList {
public:
    void addFloat(const std::string& name, const std::string& display,
                  float def, float minV, float maxV, float step);
    void addInt(const std::string& name, const std::string& display,
                int def, int minV, int maxV, int step = 1);
    void addEnum(const std::string& name, const std::string& display,
                 const std::vector<std::string>& options, int defIndex = 0);
    void addToggle(const std::string& name, const std::string& display, bool def = false);
    void addColor(const std::string& name, const std::string& display,
                  uint8_t r, uint8_t g, uint8_t b);

    // Access
    int count() const { return (int)m_params.size(); }
    Param& at(int index) { return m_params[index]; }
    const Param& at(int index) const { return m_params[index]; }
    Param* find(const std::string& name);
    const Param* find(const std::string& name) const;

    // For sequencer compatibility
    void set(const std::string& name, float value);
    float get(const std::string& name) const;

    // Iterators
    std::vector<Param>& params() { return m_params; }
    const std::vector<Param>& params() const { return m_params; }

private:
    std::vector<Param> m_params;
};
