#pragma once
#include "../config.h"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <cstdint>

#define MAX_SPRITE_SIZE 64

struct Sprite {
    uint32_t pixels[MAX_SPRITE_SIZE * MAX_SPRITE_SIZE];
    int width = 0;
    int height = 0;
    bool valid = false;

    // Draw sprite at position with optional tint and scale
    void draw(uint32_t* framebuffer, int fbW, int fbH,
              int posX, int posY, float scale = 1.0f,
              uint8_t tintR = 255, uint8_t tintG = 255, uint8_t tintB = 255,
              float alpha = 1.0f) const;
};

class SpriteCache {
public:
    static SpriteCache& instance();

    // Load sprite from BMP file, cached by path
    const Sprite* load(const std::string& path);

    // Clear cache
    void clear();

private:
    SpriteCache() = default;
    std::unordered_map<std::string, Sprite> m_cache;
};
