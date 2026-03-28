#pragma once
#include "../visual_node.h"
#include "../3d/math3d.h"
#include "../3d/rasterizer.h"
#include "../3d/camera3d.h"
#include "../3d/obj_loader.h"
#include <vector>

class SphereNode : public VisualNode {
public:
    SphereNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "SPHERE"; }
    const char* description() const override { return "3D UV sphere"; }
    NodeCategory category() const override { return NodeCategory::THREED; }

    void applyParams() override;
    void syncParams() override;

    void setCamera(Camera3D* cam) { m_camera = cam; }

    float radius = 1.0f;
    int segments = 8;
    float rotX = 0, rotY = 0, rotZ = 0;
    bool autoRotate = true;
    float rotSpeed = 0.5f;
    bool wireframe = false;

private:
    void regenerate();
    void renderMesh(Renderer& r);

    Camera3D* m_camera = nullptr;
    Rasterizer m_rasterizer;
    std::vector<Tri3D> m_triangles;
    float m_lastRadius = -1;
    int m_lastSeg = -1;
    float m_autoRotY = 0;
};
