#include "preview.h"

void PreviewPanel::render(Renderer& r, const std::vector<VisualNode*>& nodes) {
    if (fullscreen) {
        // Render nodes directly to full screen
        for (auto* node : nodes) {
            if (node && node->active) {
                node->render(r);
            }
        }
    } else {
        // Draw preview border
        r.rect(x - 1, y - 1, w + 2, h + 2, Palette::GRID, false);
        r.text(x + 2, y + h + 2, "PREVIEW", Palette::UI_FG);

        // Render nodes (they'll clip naturally since they draw within their bounds)
        for (auto* node : nodes) {
            if (node && node->active) {
                node->render(r);
            }
        }
    }
}
