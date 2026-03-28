#pragma once
#include "../visual_node.h"
#include "../3d/math3d.h"
#include "../3d/rasterizer.h"
#include "../3d/camera3d.h"
#include "../3d/obj_loader.h"
#include <vector>
#include <string>

class MeshNode : public VisualNode {
public:
    MeshNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "MESH"; }
    const char* description() const override { return "3D mesh from .obj file"; }
    NodeCategory category() const override { return NodeCategory::THREED; }

    void applyParams() override;
    void syncParams() override;

    // Set shared camera (called from main)
    void setCamera(Camera3D* cam) { m_camera = cam; }

    float meshScale = 1.0f;
    float rotX = 0, rotY = 0, rotZ = 0;
    bool autoRotate = true;
    float rotSpeed = 1.0f;
    bool wireframe = false;
    std::string objPath;

private:
    void loadMesh();
    void renderMesh(Renderer& r, const std::vector<Tri3D>& tris);

    Camera3D* m_camera = nullptr;
    Rasterizer m_rasterizer;
    std::vector<Tri3D> m_triangles;
    std::string m_loadedPath;
    float m_autoRotY = 0;
};
