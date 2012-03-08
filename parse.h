#ifndef PARSE_H
#define PARSE_H

#include <cstdlib>
#include <iostream>

#include "mesh.h"

using namespace std;

struct float3 {
    float x, y, z;
};
inline istream &operator>>(istream& is, float3& f)
{
    return is >> f.x >> f.y >> f.z;
}
inline ostream &operator<<(ostream& os, float3& f)
{
    return os << "<" << f.x << ", " << f.y << ", " << f.z << ">";
}

class Parser {
protected:
    virtual void sphere(float3 pos, float r) {}
    virtual void triangle(float3 p1, float3 p2, float3 p3) {}
    virtual void normtriangle(const Vertex*, const Vertex*, const Vertex*) {}
    virtual void plane(float3 n, float d) {}
    virtual void camera(float3 pos, float3 dir,
            float d, float iw, float ih, int pw, int ph) {}
    virtual void pointLight(float3 pos, float3 rgb) {}
    virtual void areaLight(float3 pos, float3 rgb, int samp, float rad) {}
    virtual void directionalLight(float3 dir, float3 rgb) {}
    virtual void ambientLight(float3 rgb) {}
    virtual void material(float3 diff, float3 spec, float r, float3 refl) {}
    virtual void setRecursion(int) {}
    virtual void setSample(int) {}
    virtual void setBVH() {}
    virtual void setTBB() {}

public:
    virtual void parse(const char* file);
};

#endif
