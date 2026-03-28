#pragma once
#include "../visual_node.h"

class WaveformNode : public VisualNode {
public:
    WaveformNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "WAVE"; }
    const char* description() const override { return "Oscilloscope waveform"; }

    void applyParams() override;
    void syncParams() override;

    void setWaveData(const float* samples, int count);

    int thickness = 2;
    float speed = 1.0f;

private:
    static const int WAVE_SAMPLES = 320;
    float m_wave[WAVE_SAMPLES] = {};
    float m_phase = 0.0f;
};
