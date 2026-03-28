#include "tracker_view.h"
#include <cstdio>

void TrackerView::update(Input& input, Pattern& pattern) {
    if (!visible) return;

    if (input.pressed(Button::DOWN)) {
        cursorStep = (cursorStep + 1) % pattern.length();
    }
    if (input.pressed(Button::UP)) {
        cursorStep = (cursorStep - 1 + pattern.length()) % pattern.length();
    }

    // Keep cursor in view
    if (cursorStep < m_scrollOffset) m_scrollOffset = cursorStep;
    if (cursorStep >= m_scrollOffset + m_visibleRows) m_scrollOffset = cursorStep - m_visibleRows + 1;
}

void TrackerView::render(Renderer& r, const Pattern& pattern) {
    if (!visible) return;

    int startX = 4;
    int startY = 2;
    int rowH = 8;

    // Header
    r.text(startX, startY, "STEP NODE  PARAM  VAL", Palette::UI_FG);
    r.line(startX, startY + 7, startX + 200, startY + 7, Palette::GRID);

    for (int i = 0; i < m_visibleRows && (m_scrollOffset + i) < pattern.length(); i++) {
        int step = m_scrollOffset + i;
        int yPos = startY + 10 + i * rowH;

        // Highlight current step during playback
        bool isPlayhead = pattern.isPlaying() && step == pattern.currentStep();
        bool isCursor = step == cursorStep;

        if (isCursor) {
            r.rect(startX - 2, yPos - 1, 210, rowH, {40, 40, 60}, true);
        }
        if (isPlayhead) {
            r.rect(startX - 2, yPos - 1, 210, rowH, {60, 20, 30}, true);
        }

        // Step number
        char buf[64];
        snprintf(buf, sizeof(buf), "%02X", step);
        r.text(startX, yPos, buf, isPlayhead ? Palette::RED : Palette::UI_FG);

        // Event data
        const PatternStep& s = pattern.getStep(step);
        if (s.active) {
            snprintf(buf, sizeof(buf), " N%d", s.nodeIndex);
            r.text(startX + 20, yPos, buf, Palette::CYAN);

            // Param name (truncated to 6 chars)
            std::string param = s.paramName.substr(0, 6);
            r.text(startX + 55, yPos, param, Palette::YELLOW);

            snprintf(buf, sizeof(buf), "%3.0f", s.value);
            r.text(startX + 100, yPos, buf, Palette::MAGENTA);
        } else {
            r.text(startX + 20, yPos, " ---  ---    ---", Palette::GRID);
        }
    }

    // Scrollbar
    if (pattern.length() > m_visibleRows) {
        int barH = 100;
        int barY = startY + 10;
        float ratio = (float)m_scrollOffset / (pattern.length() - m_visibleRows);
        int thumbH = std::max(8, barH * m_visibleRows / pattern.length());
        int thumbY = barY + (int)(ratio * (barH - thumbH));
        r.rect(220, barY, 3, barH, Palette::GRID);
        r.rect(220, thumbY, 3, thumbH, Palette::UI_FG);
    }
}
