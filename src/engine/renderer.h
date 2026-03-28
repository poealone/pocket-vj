#pragma once
#include <SDL2/SDL.h>
#include "../config.h"
#include <string>

class Renderer {
public:
    bool init();
    void shutdown();

    void beginFrame();
    void endFrame();

    // Primitives
    void clear(Color c = Palette::BLACK);
    void pixel(int x, int y, Color c);
    void line(int x1, int y1, int x2, int y2, Color c);
    void rect(int x, int y, int w, int h, Color c, bool filled = true);
    void rectAlpha(int x, int y, int w, int h, Color c);  // Filled rect with alpha blending
    void circle(int cx, int cy, int r, Color c, bool filled = true);

    // Bar rendering (for spectrum bars)
    void bar(int x, int y, int w, int h, Color c);

    // Text (built-in 4x6 bitmap font)
    void text(int x, int y, const std::string& str, Color c, int scale = 1);
    void textCentered(int y, const std::string& str, Color c, int scale = 1);

    // Framebuffer access (for FX)
    uint32_t* pixels() { return m_pixels; }
    int pitch() const { return RENDER_W; }

    SDL_Window* window() { return m_window; }
    float deltaTime() const { return m_dt; }
    uint32_t frameCount() const { return m_frameCount; }
    float fps() const { return m_fps; }
    bool shouldQuit() const;

private:
    SDL_Window*   m_window   = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture*  m_texture  = nullptr;
    uint32_t*     m_pixels   = nullptr;

    float    m_dt         = 0.0f;
    uint32_t m_lastTick   = 0;
    uint32_t m_frameStart = 0;
    uint32_t m_frameCount = 0;
    float    m_fpsAccum   = 0.0f;
    int      m_fpsFrames  = 0;
    float    m_fps        = 30.0f;

    void charGlyph(int x, int y, char ch, Color c, int scale);
};
