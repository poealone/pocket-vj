#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

Vec3 ObjLoader::calcNormal(const Vec3& a, const Vec3& b, const Vec3& c) {
    return (b - a).cross(c - a).normalized();
}

bool ObjLoader::load(const std::string& path, std::vector<Tri3D>& outTris) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::vector<Vec3> verts;
    outTris.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            Vec3 v;
            iss >> v.x >> v.y >> v.z;
            verts.push_back(v);
        } else if (type == "f") {
            // Parse face indices (1-based, may have v/vt/vn format)
            std::vector<int> indices;
            std::string token;
            while (iss >> token) {
                // Extract vertex index before first '/'
                int idx = std::atoi(token.c_str());
                if (idx < 0) idx = (int)verts.size() + idx + 1; // negative = relative
                indices.push_back(idx - 1); // convert to 0-based
            }

            // Triangulate (fan from first vertex)
            for (int i = 1; i + 1 < (int)indices.size(); i++) {
                int i0 = indices[0], i1 = indices[i], i2 = indices[i + 1];
                if (i0 < 0 || i0 >= (int)verts.size()) continue;
                if (i1 < 0 || i1 >= (int)verts.size()) continue;
                if (i2 < 0 || i2 >= (int)verts.size()) continue;

                Tri3D tri;
                tri.v[0] = verts[i0];
                tri.v[1] = verts[i1];
                tri.v[2] = verts[i2];
                tri.normal = calcNormal(tri.v[0], tri.v[1], tri.v[2]);
                tri.color = {200, 200, 200};
                outTris.push_back(tri);
            }
        }
    }

    if (outTris.empty()) return false;

    // Center and normalize to unit cube
    Vec3 minB = outTris[0].v[0], maxB = outTris[0].v[0];
    for (auto& tri : outTris) {
        for (int i = 0; i < 3; i++) {
            minB.x = std::min(minB.x, tri.v[i].x);
            minB.y = std::min(minB.y, tri.v[i].y);
            minB.z = std::min(minB.z, tri.v[i].z);
            maxB.x = std::max(maxB.x, tri.v[i].x);
            maxB.y = std::max(maxB.y, tri.v[i].y);
            maxB.z = std::max(maxB.z, tri.v[i].z);
        }
    }

    Vec3 center = {(minB.x + maxB.x) * 0.5f, (minB.y + maxB.y) * 0.5f, (minB.z + maxB.z) * 0.5f};
    float maxDim = std::max({maxB.x - minB.x, maxB.y - minB.y, maxB.z - minB.z});
    float scl = (maxDim > 1e-6f) ? (2.0f / maxDim) : 1.0f;

    for (auto& tri : outTris) {
        for (int i = 0; i < 3; i++) {
            tri.v[i] = (tri.v[i] - center) * scl;
        }
        tri.normal = calcNormal(tri.v[0], tri.v[1], tri.v[2]);
    }

    return true;
}

void ObjLoader::generateCube(std::vector<Tri3D>& out, float size) {
    float s = size * 0.5f;
    Vec3 v[8] = {
        {-s, -s, -s}, { s, -s, -s}, { s,  s, -s}, {-s,  s, -s},
        {-s, -s,  s}, { s, -s,  s}, { s,  s,  s}, {-s,  s,  s}
    };

    // 6 faces, 2 triangles each (CCW winding from outside)
    int faces[6][4] = {
        {0, 3, 2, 1}, // front (-Z)
        {4, 5, 6, 7}, // back (+Z)
        {0, 1, 5, 4}, // bottom (-Y)
        {2, 3, 7, 6}, // top (+Y)
        {0, 4, 7, 3}, // left (-X)
        {1, 2, 6, 5}  // right (+X)
    };

    out.clear();
    for (int f = 0; f < 6; f++) {
        Tri3D t1, t2;
        t1.v[0] = v[faces[f][0]]; t1.v[1] = v[faces[f][1]]; t1.v[2] = v[faces[f][2]];
        t2.v[0] = v[faces[f][0]]; t2.v[1] = v[faces[f][2]]; t2.v[2] = v[faces[f][3]];
        t1.normal = calcNormal(t1.v[0], t1.v[1], t1.v[2]);
        t2.normal = t1.normal;
        t1.color = t2.color = {200, 200, 200};
        out.push_back(t1);
        out.push_back(t2);
    }
}

void ObjLoader::generateSphere(std::vector<Tri3D>& out, float radius, int segments) {
    out.clear();
    int rings = segments;
    int slices = segments * 2;

    auto sphereVert = [&](int ring, int slice) -> Vec3 {
        float phi = 3.14159265f * ring / rings;
        float theta = 2.0f * 3.14159265f * slice / slices;
        return {
            radius * sinf(phi) * cosf(theta),
            radius * cosf(phi),
            radius * sinf(phi) * sinf(theta)
        };
    };

    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < slices; s++) {
            Vec3 a = sphereVert(r, s);
            Vec3 b = sphereVert(r + 1, s);
            Vec3 c = sphereVert(r + 1, s + 1);
            Vec3 d = sphereVert(r, s + 1);

            Tri3D t1;
            t1.v[0] = a; t1.v[1] = b; t1.v[2] = c;
            t1.normal = calcNormal(a, b, c);
            t1.color = {200, 200, 200};
            out.push_back(t1);

            Tri3D t2;
            t2.v[0] = a; t2.v[1] = c; t2.v[2] = d;
            t2.normal = calcNormal(a, c, d);
            t2.color = {200, 200, 200};
            out.push_back(t2);
        }
    }
}

void ObjLoader::generateTorus(std::vector<Tri3D>& out, float majorR, float minorR,
                               int majorSeg, int minorSeg) {
    out.clear();

    auto torusVert = [&](int i, int j) -> Vec3 {
        float u = 2.0f * 3.14159265f * i / majorSeg;
        float v = 2.0f * 3.14159265f * j / minorSeg;
        return {
            (majorR + minorR * cosf(v)) * cosf(u),
            minorR * sinf(v),
            (majorR + minorR * cosf(v)) * sinf(u)
        };
    };

    for (int i = 0; i < majorSeg; i++) {
        for (int j = 0; j < minorSeg; j++) {
            Vec3 a = torusVert(i, j);
            Vec3 b = torusVert(i + 1, j);
            Vec3 c = torusVert(i + 1, j + 1);
            Vec3 d = torusVert(i, j + 1);

            Tri3D t1;
            t1.v[0] = a; t1.v[1] = b; t1.v[2] = c;
            t1.normal = calcNormal(a, b, c);
            t1.color = {200, 200, 200};
            out.push_back(t1);

            Tri3D t2;
            t2.v[0] = a; t2.v[1] = c; t2.v[2] = d;
            t2.normal = calcNormal(a, c, d);
            t2.color = {200, 200, 200};
            out.push_back(t2);
        }
    }
}
