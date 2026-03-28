#include "torus.h"
#include <cmath>
#include <algorithm>

TorusNode::TorusNode() {
    color = {255, 200, 60};

    params.addFloat("major_r", "Major R", 1.0f, 0.5f, 3.0f, 0.1f);
    params.addFloat("minor_r", "Minor R", 0.3f, 0.1f, 1.0f, 0.05f);
    params.addInt("major_seg", "Maj Segs", 8, 6, 16, 1);
    params.addInt("minor_seg", "Min Segs", 6, 4, 12, 1);
    params.addFloat("rot_x", "Rot X", 0, 0, 360, 5);
    params.addFloat("rot_y", "Rot Y", 0, 0, 360, 5);
    params.addFloat("rot_z", "Rot Z", 0, 0, 360, 5);
    params.addToggle("auto_rotate", "Auto Rot", true);
    params.addFloat("rot_speed", "Rot Speed", 0.5f, 0.0f, 10.0f, 0.1f);
    params.addToggle("wireframe", "Wireframe", false);
    params.addColor("color", "Color", color.r, color.g, color.b);

    regenerate();
}

void TorusNode::applyParams() {
    Param* p;
    if ((p = params.find("major_r")))     majorRadius = p->value;
    if ((p = params.find("minor_r")))     minorRadius = p->value;
    if ((p = params.find("major_seg")))   majorSegments = (int)p->value;
    if ((p = params.find("minor_seg")))   minorSegments = (int)p->value;
    if ((p = params.find("rot_x")))       rotX = p->value;
    if ((p = params.find("rot_y")))       rotY = p->value;
    if ((p = params.find("rot_z")))       rotZ = p->value;
    if ((p = params.find("auto_rotate"))) autoRotate = p->boolValue();
    if ((p = params.find("rot_speed")))   rotSpeed = p->value;
    if ((p = params.find("wireframe")))   wireframe = p->boolValue();
    if ((p = params.find("color_r")))     color.r = (uint8_t)p->value;
    if ((p = params.find("color_g")))     color.g = (uint8_t)p->value;
    if ((p = params.find("color_b")))     color.b = (uint8_t)p->value;
}

void TorusNode::syncParams() {
    params.set("major_r", majorRadius);
    params.set("minor_r", minorRadius);
    params.set("major_seg", (float)majorSegments);
    params.set("minor_seg", (float)minorSegments);
    params.set("rot_x", rotX);
    params.set("rot_y", rotY);
    params.set("rot_z", rotZ);
    params.set("auto_rotate", autoRotate ? 1.0f : 0.0f);
    params.set("rot_speed", rotSpeed);
    params.set("wireframe", wireframe ? 1.0f : 0.0f);
    params.set("color_r", color.r);
    params.set("color_g", color.g);
    params.set("color_b", color.b);
}

void TorusNode::regenerate() {
    if (majorRadius == m_lastMajR && minorRadius == m_lastMinR &&
        majorSegments == m_lastMajS && minorSegments == m_lastMinS &&
        !m_triangles.empty()) return;
    ObjLoader::generateTorus(m_triangles, majorRadius, minorRadius, majorSegments, minorSegments);
    m_lastMajR = majorRadius;
    m_lastMinR = minorRadius;
    m_lastMajS = majorSegments;
    m_lastMinS = minorSegments;
}

void TorusNode::update(float dt, float audioLevel) {
    params.animateAll(dt);
    applyParams();
    (void)audioLevel;
    if (autoRotate) {
        m_autoRotY += rotSpeed * dt * 60.0f;
        if (m_autoRotY > 360.0f) m_autoRotY -= 360.0f;
    }
    regenerate();
}

void TorusNode::renderMesh(Renderer& r) {
    if (m_triangles.empty() || !m_camera) return;

    uint32_t* pixels = r.pixels();
    m_rasterizer.clearZBuffer();

    float deg2rad = 3.14159265f / 180.0f;
    Mat4 model = Mat4::rotateX(rotX * deg2rad)
        * Mat4::rotateY((rotY + m_autoRotY) * deg2rad)
        * Mat4::rotateZ(rotZ * deg2rad);

    Mat4 vp = m_camera->viewProjMatrix();
    Mat4 mvp = vp * model;

    Vec3 lightDir = {0.3f, 0.7f, -0.6f};
    lightDir = lightDir.normalized();

    for (const auto& tri : m_triangles) {
        Vec3 tv[3];
        float tw[3];
        for (int i = 0; i < 3; i++) {
            tv[i] = mvp.transformNoDiv(tri.v[i]);
            tw[i] = mvp.transformW(tri.v[i]);
        }

        bool behind = false;
        for (int i = 0; i < 3; i++) {
            if (tw[i] < 0.01f) { behind = true; break; }
        }
        if (behind) continue;

        Vec3 ndc[3];
        for (int i = 0; i < 3; i++) {
            ndc[i] = {tv[i].x / tw[i], tv[i].y / tw[i], tv[i].z / tw[i]};
        }

        Vec3 sv[3];
        for (int i = 0; i < 3; i++) sv[i] = projectToScreen(ndc[i]);

        float cross2d = (sv[1].x - sv[0].x) * (sv[2].y - sv[0].y) -
                         (sv[1].y - sv[0].y) * (sv[2].x - sv[0].x);
        if (cross2d < 0) continue;

        Vec3 wn = {
            model.m[0] * tri.normal.x + model.m[4] * tri.normal.y + model.m[8]  * tri.normal.z,
            model.m[1] * tri.normal.x + model.m[5] * tri.normal.y + model.m[9]  * tri.normal.z,
            model.m[2] * tri.normal.x + model.m[6] * tri.normal.y + model.m[10] * tri.normal.z
        };
        wn = wn.normalized();

        float ndotl = wn.dot(lightDir);
        float shade = std::max(0.15f, std::min(1.0f, ndotl * 0.7f + 0.3f));

        Color fc = {(uint8_t)(color.r * shade), (uint8_t)(color.g * shade), (uint8_t)(color.b * shade)};

        if (wireframe)
            m_rasterizer.drawTriangleWireframe(sv[0], sv[1], sv[2], fc, pixels);
        else
            m_rasterizer.drawTriangle(sv[0], sv[1], sv[2], fc, pixels);
    }
}

void TorusNode::render(Renderer& r) {
    if (!active) return;
    renderMesh(r);
}
