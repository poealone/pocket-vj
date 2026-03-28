#include "mesh.h"
#include <cmath>
#include <algorithm>

MeshNode::MeshNode() {
    color = {200, 100, 255};

    params.addFloat("scale", "Scale", 1.0f, 0.1f, 10.0f, 0.1f);
    params.addFloat("rot_x", "Rot X", 0, 0, 360, 5);
    params.addFloat("rot_y", "Rot Y", 0, 0, 360, 5);
    params.addFloat("rot_z", "Rot Z", 0, 0, 360, 5);
    params.addToggle("auto_rotate", "Auto Rot", true);
    params.addFloat("rot_speed", "Rot Speed", 1.0f, 0.0f, 10.0f, 0.1f);
    params.addToggle("wireframe", "Wireframe", false);
    params.addColor("color", "Color", color.r, color.g, color.b);

    // Default mesh — try crystal
    objPath = "meshes/crystal.obj";
}

void MeshNode::applyParams() {
    Param* p;
    if ((p = params.find("scale")))       meshScale = p->value;
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

void MeshNode::syncParams() {
    params.set("scale", meshScale);
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

void MeshNode::loadMesh() {
    if (objPath == m_loadedPath && !m_triangles.empty()) return;
    m_triangles.clear();
    ObjLoader::load(objPath, m_triangles);
    m_loadedPath = objPath;
}

void MeshNode::update(float dt, float audioLevel) {
    params.animateAll(dt);
    applyParams();
    (void)audioLevel;
    if (autoRotate) {
        m_autoRotY += rotSpeed * dt * 60.0f;
        if (m_autoRotY > 360.0f) m_autoRotY -= 360.0f;
    }
    loadMesh();
}

void MeshNode::renderMesh(Renderer& r, const std::vector<Tri3D>& tris) {
    if (tris.empty() || !m_camera) return;

    uint32_t* pixels = r.pixels();
    m_rasterizer.clearZBuffer();

    // Build model matrix
    float deg2rad = 3.14159265f / 180.0f;
    Mat4 model = Mat4::scale(meshScale)
        * Mat4::rotateX(rotX * deg2rad)
        * Mat4::rotateY((rotY + m_autoRotY) * deg2rad)
        * Mat4::rotateZ(rotZ * deg2rad);

    Mat4 vp = m_camera->viewProjMatrix();
    Mat4 mvp = vp * model;

    // Light direction (view space)
    Vec3 lightDir = {0.3f, 0.7f, -0.6f};
    lightDir = lightDir.normalized();

    for (const auto& tri : tris) {
        // Transform vertices
        Vec3 tv[3];
        float tw[3];
        for (int i = 0; i < 3; i++) {
            // Model transform for normal calculation
            Vec3 mv = model.transformNoDiv(tri.v[i]);
            (void)mv;
            // Full MVP
            tv[i] = mvp.transformNoDiv(tri.v[i]);
            tw[i] = mvp.transformW(tri.v[i]);
        }

        // Near-plane clip: skip if any vertex behind camera
        bool behind = false;
        for (int i = 0; i < 3; i++) {
            if (tw[i] < 0.01f) { behind = true; break; }
        }
        if (behind) continue;

        // Perspective divide
        Vec3 ndc[3];
        for (int i = 0; i < 3; i++) {
            ndc[i] = {tv[i].x / tw[i], tv[i].y / tw[i], tv[i].z / tw[i]};
        }

        // Backface cull in screen space
        Vec3 sv[3];
        for (int i = 0; i < 3; i++) {
            sv[i] = projectToScreen(ndc[i]);
        }

        float cross2d = (sv[1].x - sv[0].x) * (sv[2].y - sv[0].y) -
                         (sv[1].y - sv[0].y) * (sv[2].x - sv[0].x);
        if (cross2d < 0) continue; // backface

        // Flat shading — transform normal by model matrix (no translate)
        Vec3 wn = {
            model.m[0] * tri.normal.x + model.m[4] * tri.normal.y + model.m[8]  * tri.normal.z,
            model.m[1] * tri.normal.x + model.m[5] * tri.normal.y + model.m[9]  * tri.normal.z,
            model.m[2] * tri.normal.x + model.m[6] * tri.normal.y + model.m[10] * tri.normal.z
        };
        wn = wn.normalized();

        float ndotl = wn.dot(lightDir);
        float shade = std::max(0.15f, std::min(1.0f, ndotl * 0.7f + 0.3f));

        Color fc = {
            (uint8_t)(color.r * shade),
            (uint8_t)(color.g * shade),
            (uint8_t)(color.b * shade)
        };

        if (wireframe) {
            m_rasterizer.drawTriangleWireframe(sv[0], sv[1], sv[2], fc, pixels);
        } else {
            m_rasterizer.drawTriangle(sv[0], sv[1], sv[2], fc, pixels);
        }
    }
}

void MeshNode::render(Renderer& r) {
    if (!active) return;
    renderMesh(r, m_triangles);
}
