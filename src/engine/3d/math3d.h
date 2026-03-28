#pragma once
#include <cmath>
#include "../../config.h"

struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    float length() const { return sqrtf(x * x + y * y + z * z); }
    Vec3 normalized() const {
        float l = length();
        if (l < 1e-6f) return {0, 0, 0};
        return {x / l, y / l, z / l};
    }
};

struct Mat4 {
    float m[16]; // column-major

    Mat4() { identity(); }

    void identity() {
        for (int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Column-major: m[col*4 + row]
    float at(int row, int col) const { return m[col * 4 + row]; }
    float& at(int row, int col) { return m[col * 4 + row]; }

    Mat4 operator*(const Mat4& b) const {
        Mat4 r;
        for (int i = 0; i < 16; i++) r.m[i] = 0;
        for (int col = 0; col < 4; col++)
            for (int row = 0; row < 4; row++)
                for (int k = 0; k < 4; k++)
                    r.m[col * 4 + row] += m[k * 4 + row] * b.m[col * 4 + k];
        return r;
    }

    Vec3 transform(const Vec3& v) const {
        float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];
        if (fabsf(w) < 1e-6f) w = 1e-6f;
        return {
            (m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12]) / w,
            (m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13]) / w,
            (m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]) / w
        };
    }

    // Transform without perspective divide (for getting clip-space z)
    Vec3 transformNoDiv(const Vec3& v) const {
        return {
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]
        };
    }

    float transformW(const Vec3& v) const {
        return m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];
    }

    static Mat4 translate(float x, float y, float z) {
        Mat4 r;
        r.m[12] = x; r.m[13] = y; r.m[14] = z;
        return r;
    }

    static Mat4 scale(float s) {
        Mat4 r;
        r.m[0] = s; r.m[5] = s; r.m[10] = s;
        return r;
    }

    static Mat4 rotateX(float angle) {
        Mat4 r;
        float c = cosf(angle), s = sinf(angle);
        r.m[5] = c;  r.m[9]  = -s;
        r.m[6] = s;  r.m[10] = c;
        return r;
    }

    static Mat4 rotateY(float angle) {
        Mat4 r;
        float c = cosf(angle), s = sinf(angle);
        r.m[0] = c;  r.m[8]  = s;
        r.m[2] = -s; r.m[10] = c;
        return r;
    }

    static Mat4 rotateZ(float angle) {
        Mat4 r;
        float c = cosf(angle), s = sinf(angle);
        r.m[0] = c;  r.m[4] = -s;
        r.m[1] = s;  r.m[5] = c;
        return r;
    }

    static Mat4 perspective(float fovDeg, float aspect, float near, float far) {
        Mat4 r;
        for (int i = 0; i < 16; i++) r.m[i] = 0;
        float fovRad = fovDeg * 3.14159265f / 180.0f;
        float tanHalf = tanf(fovRad * 0.5f);
        r.m[0]  = 1.0f / (aspect * tanHalf);
        r.m[5]  = 1.0f / tanHalf;
        r.m[10] = -(far + near) / (far - near);
        r.m[11] = -1.0f;
        r.m[14] = -(2.0f * far * near) / (far - near);
        return r;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).normalized();
        Vec3 r = f.cross(up).normalized();
        Vec3 u = r.cross(f);

        Mat4 m;
        m.m[0] = r.x;  m.m[4] = r.y;  m.m[8]  = r.z;  m.m[12] = -r.dot(eye);
        m.m[1] = u.x;  m.m[5] = u.y;  m.m[9]  = u.z;  m.m[13] = -u.dot(eye);
        m.m[2] = -f.x; m.m[6] = -f.y; m.m[10] = -f.z; m.m[14] = f.dot(eye);
        m.m[3] = 0;    m.m[7] = 0;    m.m[11] = 0;    m.m[15] = 1.0f;
        return m;
    }
};

struct Tri3D {
    Vec3 v[3];
    Vec3 normal;
    Color color;
};

// Project a clip-space point to screen coordinates
inline Vec3 projectToScreen(const Vec3& ndc) {
    return {
        (ndc.x * 0.5f + 0.5f) * RENDER_W,
        (1.0f - (ndc.y * 0.5f + 0.5f)) * RENDER_H,
        ndc.z
    };
}
