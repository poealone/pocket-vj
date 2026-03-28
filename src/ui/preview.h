#pragma once
#include "../engine/renderer.h"
#include "../engine/visual_node.h"
#include <vector>

// Live visual preview panel
class PreviewPanel {
public:
    void render(Renderer& r, const std::vector<VisualNode*>& nodes);

    bool fullscreen = false;  // Toggle to show preview fullscreen

    // Preview area (right side of tracker, or fullscreen)
    int x = 226, y = 2, w = 90, h = 90;
};
