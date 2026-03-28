#pragma once
#include <cstdint>
#include <algorithm>

// Screen defaults (overridden by CMake defines)
#ifndef SCREEN_W
#define SCREEN_W 320
#endif
#ifndef SCREEN_H
#define SCREEN_H 240
#endif
#ifndef TARGET_FPS
#define TARGET_FPS 30
#endif

// Internal render resolution (always 320x240, scaled up for desktop)
#define RENDER_W 320
#define RENDER_H 240

// Visual engine limits
#define MAX_LAYERS      8
#define MAX_NODES       32
#define MAX_PARTICLES   128
#define MAX_FFT_BINS    32
#define MAX_PATTERN_LEN 64

// Colors (RGB565 for software renderer, but we store as RGB888 internally)
struct Color {
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

    uint32_t toSDL() const { return (a << 24) | (r << 16) | (g << 8) | b; }
};

// Common palettes
namespace Palette {
    static const Color BLACK   = {0, 0, 0};
    static const Color WHITE   = {255, 255, 255};
    static const Color RED     = {255, 30, 60};
    static const Color CYAN    = {60, 255, 180};
    static const Color BLUE    = {30, 60, 255};
    static const Color MAGENTA = {255, 60, 200};
    static const Color YELLOW  = {255, 220, 30};
    static const Color GRID    = {40, 40, 50};
    static const Color UI_BG   = {18, 18, 24};
    static const Color UI_FG   = {200, 200, 210};
    static const Color UI_HL   = {255, 30, 60};
}

// App info
#define APP_NAME    "Pocket VJ"
#define APP_VERSION "0.1.0"
