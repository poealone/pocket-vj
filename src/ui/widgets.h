#pragma once
#include "../engine/renderer.h"
#include "../engine/param.h"

namespace Widgets {

// Horizontal slider bar with value text. Returns width used.
// w = total width of slider area
void drawSlider(Renderer& r, int x, int y, int w, const Param& p, bool selected = false);

// Rotary knob display (visual only, shows angle)
void drawKnob(Renderer& r, int x, int y, const Param& p, bool selected = false);

// Enum selector: ◀ VALUE ▶
void drawEnum(Renderer& r, int x, int y, const Param& p, bool selected = false);

// Toggle: [ON] / [OFF]
void drawToggle(Renderer& r, int x, int y, const Param& p, bool selected = false);

// Color swatch: filled square
void drawColorSwatch(Renderer& r, int x, int y, uint8_t cr, uint8_t cg, uint8_t cb);

// Draw any param with the appropriate widget
void drawParam(Renderer& r, int x, int y, int w, const Param& p, bool selected = false);

} // namespace Widgets
