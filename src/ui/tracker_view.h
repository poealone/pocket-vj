#pragma once
#include "../engine/renderer.h"
#include "../sequencer/pattern.h"
#include "../input.h"

// Main tracker grid UI
class TrackerView {
public:
    void update(Input& input, Pattern& pattern);
    void render(Renderer& r, const Pattern& pattern);

    int cursorStep = 0;
    bool visible = true;

private:
    int m_scrollOffset = 0;
    int m_visibleRows = 12;
};
