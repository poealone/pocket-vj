#include "bpm.h"
#include <cmath>

void BpmTap::tap() {
    Uint32 now = SDL_GetTicks();

    // If too long since last tap, reset
    if (m_tapCount > 0) {
        int lastIdx = (m_tapIndex - 1 + MAX_TAPS) % MAX_TAPS;
        if (now - m_taps[lastIdx] > TIMEOUT_MS) {
            reset();
        }
    }

    m_taps[m_tapIndex] = now;
    m_tapIndex = (m_tapIndex + 1) % MAX_TAPS;
    if (m_tapCount < MAX_TAPS) m_tapCount++;
}

float BpmTap::getBpm() const {
    if (m_tapCount < 2) return 120.0f;  // Default

    // Calculate average interval from recorded taps
    // Taps are in a ring buffer; find the ordered sequence
    int count = m_tapCount;
    Uint32 ordered[MAX_TAPS];
    for (int i = 0; i < count; i++) {
        int idx = (m_tapIndex - count + i + MAX_TAPS) % MAX_TAPS;
        ordered[i] = m_taps[idx];
    }

    // Average the intervals
    Uint32 totalMs = ordered[count - 1] - ordered[0];
    float avgInterval = (float)totalMs / (float)(count - 1);

    if (avgInterval < 1.0f) return 120.0f;

    float bpm = 60000.0f / avgInterval;

    // Clamp to reasonable range
    if (bpm < 30.0f) bpm = 30.0f;
    if (bpm > 300.0f) bpm = 300.0f;

    return roundf(bpm);
}

bool BpmTap::isValid() const {
    if (m_tapCount < 2) return false;

    Uint32 now = SDL_GetTicks();
    int lastIdx = (m_tapIndex - 1 + MAX_TAPS) % MAX_TAPS;
    return (now - m_taps[lastIdx]) < TIMEOUT_MS;
}

void BpmTap::reset() {
    m_tapCount = 0;
    m_tapIndex = 0;
}
