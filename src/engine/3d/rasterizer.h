#pragma once
#include "math3d.h"
#include "../../config.h"
#include <cstdint>

class Rasterizer {
public:
    Rasterizer();
    ~Rasterizer();

    void clearZBuffer();

    // Filled triangle with flat shading + z-buffer
    void drawTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                      Color color, uint32_t* pixels);

    // Wireframe triangle (no z-buffer)
    void drawTriangleWireframe(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                               Color color, uint32_t* pixels);

    float* zbuffer() { return m_zbuf; }

private:
    float* m_zbuf;

    void drawLine(int x0, int y0, int x1, int y1, Color c, uint32_t* pixels);
};
