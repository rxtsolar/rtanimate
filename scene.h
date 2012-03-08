#ifndef SCENE_H
#define SCENE_H

#include <cfloat>
#include <vector>
#include "vector3.h"
#include "parse.h"

typedef Vector3f Point;
typedef Vector3f Direct;
typedef Vector3f Color;

struct Ray;
struct BoundingBox;
struct Camera;
struct Material;
struct TreeNode;
class BvhTree;
class Object;
class Plane;
class Sphere;
class Triangle;
class NormTriangle;
class Light;
class PointLight;
class DirectionalLight;
class AmbientLight;

struct Ray {
    Point pt;
    Direct dir;
    Ray(Point p, Direct d) { pt = p; dir = d.normalize(); }
};

struct BoundingBox {
    BoundingBox() : pMin(FLT_MAX, FLT_MAX, FLT_MAX),
                    pMax(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}
    Point pMin;
    Point pMax;
    bool isHit(const Ray&) const;
};

struct Camera {
    Point pos;
    Direct dir;
    float d;
    float iw;
    float ih;
    int pw;
    int ph;
};

struct Material {
    Color diff;
    Color spec;
    float r;
    Color refl;
    Material& operator=(const Material&);
};

class Object {
public:
    float key;
    BoundingBox bbox;
    virtual Direct getNormal(const Point&) const = 0;
    virtual bool isHit(const Ray&, Point&, float&) const = 0;
    virtual bool isBounded() = 0;
    Material mat;
};

struct TreeNode {
    BoundingBox bbox;
    TreeNode* left;
    TreeNode* right;
    Object* obj;
};

class Light {
public:
    virtual Color getColor(const Point&, const Direct&,
            const Direct&, const Material&, const vector<Object*>& objects,
            const vector<Object*>& planes, const BvhTree& tree) const = 0;
protected:
    bool isReachable(const Point&, const std::vector<Object*>&,
            const std::vector<Object*>&, const BvhTree&) const { return true; }
    Color rgb;
};

class Sphere : public Object {
public:
    Sphere(float3, float);
    virtual Direct getNormal(const Point&) const;
    virtual bool isHit(const Ray&, Point&, float&) const;
    virtual bool isBounded() { return true; }
protected:
    Point pos;
    float r;
};

class Triangle : public Object {
public:
    float key;
    Triangle() {};
    Triangle(float3, float3, float3);
    virtual Direct getNormal(const Point&) const;
    virtual bool isHit(const Ray&, Point&, float&) const;
    virtual bool isBounded() { return true; }
protected:
    Point p1;
    Point p2;
    Point p3;
    bool isInside(const Point&) const;
};

class NormTriangle : public Triangle {
public:
    NormTriangle(const Vertex*, const Vertex*, const Vertex*);
    virtual Direct getNormal(const Point&) const;
protected:
    Direct n1;
    Direct n2;
    Direct n3;
};

class Plane : public Object {
public:
    Plane(float3, float);
    Plane(float, float, float, float);
    Plane(const Point&, const Point&, const Point&);
    virtual Direct getNormal(const Point&) const;
    virtual bool isHit(const Ray&, Point&, float&) const;
    virtual bool isBounded() { return false; }
protected:
    Direct n;
    float d;
};

class PointLight : public Light {
public:
    PointLight(float3, float3);
    virtual Color getColor(const Point&, const Direct&,
            const Direct&, const Material&, const vector<Object*>& objects,
            const vector<Object*>& planes, const BvhTree& tree) const;
protected:
    bool isReachable(const Point&, const std::vector<Object*>&,
            const std::vector<Object*>&, const BvhTree&) const;
    Point pos;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(float3, float3);
    virtual Color getColor(const Point&, const Direct&,
            const Direct&, const Material&, const vector<Object*>& objects,
            const vector<Object*>& planes, const BvhTree& tree) const;
protected:
    bool isReachable(const Point&, const std::vector<Object*>&,
            const std::vector<Object*>&, const BvhTree&) const;
    Direct dir;
};

class AmbientLight : public Light {
public:
    AmbientLight(float3);
    virtual Color getColor(const Point&, const Direct&,
            const Direct&, const Material&, const vector<Object*>& objects,
            const vector<Object*>& planes, const BvhTree& tree) const;
};

class AreaLight : public PointLight {
public:
    AreaLight(float3, float3, int, float);
    virtual Color getColor(const Point&, const Direct&,
            const Direct&, const Material&, const vector<Object*>& objects,
            const vector<Object*>& planes, const BvhTree& tree) const;
protected:
    bool isReachable(const Point&, const std::vector<Object*>&,
            const std::vector<Object*>&, const BvhTree&, const Point&) const;
    int samp;
    float rad;
};

class BvhTree {
public:
    BvhTree() : root(0), valid(false) {}
    ~BvhTree() { freeTree(root); }
    vector<Object*> buildTree(const vector<Object*>&, const BoundingBox&);
    vector<Object*> getPossibleObjs(const Ray&) const;
    bool isValid() const { return valid; }
private:
    TreeNode* root;
    bool valid;

    void getObjs(const TreeNode*, const Ray&, vector<Object*>&) const;
    void freeTree(TreeNode*);
};

float frand();

#endif
