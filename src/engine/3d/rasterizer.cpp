#include "rasterizer.h"
#include <algorithm>
#include <cstring>
#include <cmath>

Rasterizer::Rasterizer() {
    m_zbuf = new float[RENDER_W * RENDER_H];
    clearZBuffer();
}

Rasterizer::~Rasterizer() {
    delete[] m_zbuf;
}

void Rasterizer::clearZBuffer() {
    for (int i = 0; i < RENDER_W * RENDER_H; i++)
        m_zbuf[i] = 1e30f;
}

void Rasterizer::drawTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                               Color color, uint32_t* pixels) {
    // Sort vertices by y (top to bottom)
    Vec3 sv[3] = {v0, v1, v2};
    if (sv[0].y > sv[1].y) std::swap(sv[0], sv[1]);
    if (sv[1].y > sv[2].y) std::swap(sv[1], sv[2]);
    if (sv[0].y > sv[1].y) std::swap(sv[0], sv[1]);

    int y0 = (int)ceilf(sv[0].y);
    int y1 = (int)ceilf(sv[1].y);
    int y2 = (int)ceilf(sv[2].y);

    y0 = std::max(y0, 0);
    y2 = std::min(y2, RENDER_H - 1);

    uint32_t col32 = color.toSDL();

    float totalHeight = sv[2].y - sv[0].y;
    if (totalHeight < 0.5f) return;

    // Scanline rasterization
    for (int y = y0; y <= y2; y++) {
        bool secondHalf = y >= y1;
        float segH = secondHalf ? (sv[2].y - sv[1].y) : (sv[1].y - sv[0].y);
        if (segH < 0.5f) segH = 0.5f;

        float alpha = ((float)y - sv[0].y) / totalHeight;
        float beta  = secondHalf ?
            ((float)y - sv[1].y) / segH :
            ((float)y - sv[0].y) / segH;

        alpha = std::max(0.0f, std::min(1.0f, alpha));
        beta  = std::max(0.0f, std::min(1.0f, beta));

        // Interpolate x and z along edges
        float ax = sv[0].x + (sv[2].x - sv[0].x) * alpha;
        float az = sv[0].z + (sv[2].z - sv[0].z) * alpha;

        float bx, bz;
        if (secondHalf) {
            bx = sv[1].x + (sv[2].x - sv[1].x) * beta;
            bz = sv[1].z + (sv[2].z - sv[1].z) * beta;
        } else {
            bx = sv[0].x + (sv[1].x - sv[0].x) * beta;
            bz = sv[0].z + (sv[1].z - sv[0].z) * beta;
        }

        if (ax > bx) { std::swap(ax, bx); std::swap(az, bz); }

        int xStart = std::max((int)ceilf(ax), 0);
        int xEnd   = std::min((int)floorf(bx), RENDER_W - 1);

        float spanW = bx - ax;
        if (spanW < 0.5f) spanW = 0.5f;

        for (int x = xStart; x <= xEnd; x++) {
            float t = (spanW > 0.5f) ? ((float)x - ax) / spanW : 0.0f;
            float z = az + (bz - az) * t;

            int idx = y * RENDER_W + x;
            if (z < m_zbuf[idx]) {
                m_zbuf[idx] = z;
                pixels[idx] = col32;
            }
        }
    }
}

void Rasterizer::drawLine(int x0, int y0, int x1, int y1, Color c, uint32_t* pixels) {
    uint32_t col32 = c.toSDL();
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < RENDER_W && y0 >= 0 && y0 < RENDER_H)
            pixels[y0 * RENDER_W + x0] = col32;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void Rasterizer::drawTriangleWireframe(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                                        Color color, uint32_t* pixels) {
    drawLine((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, color, pixels);
    drawLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, color, pixels);
    drawLine((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, color, pixels);
}
