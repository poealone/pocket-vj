#pragma once
#include "math3d.h"
#include "../param.h"

class Camera3D {
public:
    Camera3D();

    void update(float dt);

    Mat4 viewMatrix() const;
    Mat4 projMatrix() const;
    Mat4 viewProjMatrix() const;

    // Params for UI editing
    ParamList params;
    void applyParams();
    void syncParams();

    // Camera state
    float fov       = 60.0f;
    float distance  = 5.0f;
    float orbitSpeed = 1.0f;
    float elevation = 20.0f;
    bool  autoOrbit = true;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;

private:
    float m_orbitAngle = 0.0f;
};
