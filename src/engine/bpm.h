#pragma once
#include <SDL2/SDL.h>

// BPM Tap System — tap to set tempo
class BpmTap {
public:
    void tap();                    // Call on each tap
    float getBpm() const;          // Return calculated BPM
    bool isValid() const;          // True if enough recent taps
    void reset();                  // Clear tap history

    static constexpr int MAX_TAPS = 8;
    static constexpr Uint32 TIMEOUT_MS = 3000;  // Reset if no tap for 3s

private:
    Uint32 m_taps[MAX_TAPS] = {};
    int m_tapCount = 0;
    int m_tapIndex = 0;            // Ring buffer write index
};
