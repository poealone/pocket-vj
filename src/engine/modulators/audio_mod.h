#pragma once
#include "../visual_node.h"
#include "../audio/fft.h"

enum class AudioSource { BASS, MID, HIGH, LEVEL, BEAT };

class AudioModulator : public VisualNode {
public:
    AudioModulator();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "AMOD"; }
    const char* description() const override { return "Audio-reactive mod"; }
    NodeCategory category() const override { return NodeCategory::MODULATOR; }

    void applyParams() override;
    void syncParams() override;

    // Output value (0-1)
    float output() const { return m_output; }

    // Must be called each frame with current FFT data
    void feed(const FFTAnalyzer& fft);

    AudioSource source = AudioSource::BASS;
    float gain = 1.0f;
    float smoothing = 0.3f;

private:
    float m_output = 0.0f;
    float m_smoothed = 0.0f;
};
