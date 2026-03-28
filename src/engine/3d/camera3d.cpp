#include "camera3d.h"
#include <cmath>

Camera3D::Camera3D() {
    params.addFloat("fov", "FOV", 60.0f, 30.0f, 120.0f, 5.0f);
    params.addFloat("distance", "Distance", 5.0f, 1.0f, 20.0f, 0.5f);
    params.addFloat("orbit_speed", "Orbit Spd", 1.0f, 0.0f, 5.0f, 0.1f);
    params.addFloat("elevation", "Elevation", 20.0f, -89.0f, 89.0f, 5.0f);
    params.addToggle("auto_orbit", "Auto Orbit", true);
}

void Camera3D::applyParams() {
    Param* p;
    if ((p = params.find("fov")))         fov = p->value;
    if ((p = params.find("distance")))    distance = p->value;
    if ((p = params.find("orbit_speed"))) orbitSpeed = p->value;
    if ((p = params.find("elevation")))   elevation = p->value;
    if ((p = params.find("auto_orbit")))  autoOrbit = p->boolValue();
}

void Camera3D::syncParams() {
    params.set("fov", fov);
    params.set("distance", distance);
    params.set("orbit_speed", orbitSpeed);
    params.set("elevation", elevation);
    params.set("auto_orbit", autoOrbit ? 1.0f : 0.0f);
}

void Camera3D::update(float dt) {
    if (autoOrbit) {
        m_orbitAngle += orbitSpeed * dt;
        if (m_orbitAngle > 6.28318f) m_orbitAngle -= 6.28318f;
    }
}

Mat4 Camera3D::viewMatrix() const {
    float elevRad = elevation * 3.14159265f / 180.0f;
    float cx = distance * cosf(elevRad) * cosf(m_orbitAngle);
    float cy = distance * sinf(elevRad);
    float cz = distance * cosf(elevRad) * sinf(m_orbitAngle);

    Vec3 eye = {cx, cy, cz};
    Vec3 target = {0, 0, 0};
    Vec3 up = {0, 1, 0};

    return Mat4::lookAt(eye, target, up);
}

Mat4 Camera3D::projMatrix() const {
    float aspect = (float)RENDER_W / (float)RENDER_H;
    return Mat4::perspective(fov, aspect, nearPlane, farPlane);
}

Mat4 Camera3D::viewProjMatrix() const {
    return projMatrix() * viewMatrix();
}
