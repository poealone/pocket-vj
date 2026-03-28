#include "sprite.h"
#include <cstring>
#include <algorithm>

SpriteCache& SpriteCache::instance() {
    static SpriteCache cache;
    return cache;
}

const Sprite* SpriteCache::load(const std::string& path) {
    if (path.empty()) return nullptr;

    // Check cache
    auto it = m_cache.find(path);
    if (it != m_cache.end()) {
        return it->second.valid ? &it->second : nullptr;
    }

    // Load BMP
    Sprite sprite;
    sprite.valid = false;

    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        m_cache[path] = sprite;
        return nullptr;
    }

    // Clamp to max size
    int srcW = std::min(surface->w, MAX_SPRITE_SIZE);
    int srcH = std::min(surface->h, MAX_SPRITE_SIZE);
    sprite.width = srcW;
    sprite.height = srcH;

    // Convert to ARGB8888
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);

    if (!converted) {
        m_cache[path] = sprite;
        return nullptr;
    }

    // Copy pixels with color key transparency (magenta #FF00FF)
    uint32_t* srcPx = (uint32_t*)converted->pixels;
    int srcPitch = converted->pitch / 4;

    memset(sprite.pixels, 0, sizeof(sprite.pixels));
    for (int y = 0; y < srcH; y++) {
        for (int x = 0; x < srcW; x++) {
            uint32_t p = srcPx[y * srcPitch + x];
            uint8_t r = (p >> 16) & 0xFF;
            uint8_t g = (p >> 8) & 0xFF;
            uint8_t b = p & 0xFF;

            // Magenta = transparent
            if (r == 255 && g == 0 && b == 255) {
                sprite.pixels[y * MAX_SPRITE_SIZE + x] = 0; // transparent
            } else {
                sprite.pixels[y * MAX_SPRITE_SIZE + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }
    }

    SDL_FreeSurface(converted);
    sprite.valid = true;
    m_cache[path] = sprite;
    return &m_cache[path];
}

void SpriteCache::clear() {
    m_cache.clear();
}

void Sprite::draw(uint32_t* framebuffer, int fbW, int fbH,
                  int posX, int posY, float scale,
                  uint8_t tintR, uint8_t tintG, uint8_t tintB,
                  float alpha) const {
    if (!valid) return;

    int drawW = (int)(width * scale);
    int drawH = (int)(height * scale);

    // Center the sprite on posX, posY
    int startX = posX - drawW / 2;
    int startY = posY - drawH / 2;

    for (int dy = 0; dy < drawH; dy++) {
        int srcY = (int)(dy / scale);
        if (srcY >= height) srcY = height - 1;
        int screenY = startY + dy;
        if (screenY < 0 || screenY >= fbH) continue;

        for (int dx = 0; dx < drawW; dx++) {
            int srcX = (int)(dx / scale);
            if (srcX >= width) srcX = width - 1;
            int screenX = startX + dx;
            if (screenX < 0 || screenX >= fbW) continue;

            uint32_t p = pixels[srcY * MAX_SPRITE_SIZE + srcX];
            if ((p & 0xFF000000) == 0) continue; // transparent

            uint8_t pr = (p >> 16) & 0xFF;
            uint8_t pg = (p >> 8) & 0xFF;
            uint8_t pb = p & 0xFF;

            // Apply tint
            pr = (uint8_t)((pr * tintR) / 255);
            pg = (uint8_t)((pg * tintG) / 255);
            pb = (uint8_t)((pb * tintB) / 255);

            if (alpha < 1.0f) {
                uint32_t dst = framebuffer[screenY * fbW + screenX];
                uint8_t dr = (dst >> 16) & 0xFF;
                uint8_t dg = (dst >> 8) & 0xFF;
                uint8_t db = dst & 0xFF;
                pr = (uint8_t)(pr * alpha + dr * (1.0f - alpha));
                pg = (uint8_t)(pg * alpha + dg * (1.0f - alpha));
                pb = (uint8_t)(pb * alpha + db * (1.0f - alpha));
            }

            framebuffer[screenY * fbW + screenX] = 0xFF000000 | (pr << 16) | (pg << 8) | pb;
        }
    }
}
