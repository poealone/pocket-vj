#pragma once
#include "math3d.h"
#include <vector>
#include <string>

class ObjLoader {
public:
    // Load .obj file, returns triangles centered and normalized to unit cube
    static bool load(const std::string& path, std::vector<Tri3D>& outTris);

    // Generate primitive geometry
    static void generateCube(std::vector<Tri3D>& out, float size = 1.0f);
    static void generateSphere(std::vector<Tri3D>& out, float radius = 1.0f, int segments = 8);
    static void generateTorus(std::vector<Tri3D>& out, float majorR = 1.0f, float minorR = 0.3f,
                              int majorSeg = 8, int minorSeg = 6);

private:
    static Vec3 calcNormal(const Vec3& a, const Vec3& b, const Vec3& c);
};
