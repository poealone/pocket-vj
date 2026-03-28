#pragma once
#include "../config.h"
#include <string>

// A single step event in a visual pattern
struct PatternStep {
    bool active = false;
    int  nodeIndex = -1;       // Which visual node to affect
    std::string paramName;     // Parameter to change
    float value = 0.0f;       // Value to set
};

// A visual pattern (like a tracker pattern)
class Pattern {
public:
    Pattern(int length = 16);

    void setStep(int step, int nodeIdx, const std::string& param, float value);
    void clearStep(int step);
    const PatternStep& getStep(int step) const;

    int length() const { return m_length; }
    void setLength(int len);

    // Playback
    void tick();                    // Advance one step
    void reset();
    int currentStep() const { return m_currentStep; }
    bool isPlaying() const { return m_playing; }
    void setPlaying(bool p) { m_playing = p; }
    void togglePlaying() { m_playing = !m_playing; }

    // BPM
    float bpm() const { return m_bpm; }
    void setBpm(float bpm) { m_bpm = bpm; }

    // Check if current step has an event
    bool hasEvent() const;
    const PatternStep& currentEvent() const;

    // Time management
    void update(float dt);

private:
    PatternStep m_steps[MAX_PATTERN_LEN];
    int   m_length = 16;
    int   m_currentStep = 0;
    bool  m_playing = false;
    float m_bpm = 120.0f;
    float m_accumulator = 0.0f;
};
