#include <cfloat>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "scene.h"

float EPSILON = 0.01;

bool BoundingBox::isHit(const Ray& ray) const
{
    float tMin = EPSILON;
    float tMax = FLT_MAX;
    for (int i = 0; i < 3; i++) {
        float d;
        float t;

        d = ray.dir[i];
        if (d == 0) {
            if (pMin[i] - ray.pt[i] > 0)
                t = FLT_MAX;
            else
                t = -FLT_MAX;
        } else if (d > 0) {
            t = (pMin[i] - ray.pt[i]) / d;
        } else {
            t = (pMax[i] - ray.pt[i]) / d;
        }
        tMin = tMin > t ? tMin : t;

        d = ray.dir[i];
        if (d == 0) {
            if (pMin[i] - ray.pt[i] > 0)
                t = FLT_MAX;
            else
                t = -FLT_MAX;
        } else if (d > 0) {
            t = (pMax[i] - ray.pt[i]) / d;
        } else {
            t = (pMin[i] - ray.pt[i]) / d;
        }
        tMax = tMax < t ? tMax : t;

        if (tMin > tMax)
            return false;
    }
    return true;
}

Material& Material::operator=(const Material& m)
{
    diff = m.diff;
    spec = m.spec;
    r = m.r;
    refl = m.refl;
    return *this;
}

Sphere::Sphere(float3 pos, float r)
{
    this->pos = Point(pos.x, pos.y, pos.z);
    this->r = r;
    bbox.pMin = this->pos - Direct(r, r, r);
    bbox.pMax = this->pos + Direct(r, r, r);
}

Direct Sphere::getNormal(const Point& p) const
{
    return (p - pos).normalize();
}

bool Sphere::isHit(const Ray& ray, Point& point, float& t) const
{
    float t1;
    float t2;
    float desc = ray.dir.dot(ray.pt - pos);
    desc *= desc;
    desc -= ray.dir.dot(ray.dir)*((ray.pt - pos).dot(ray.pt - pos) - r*r);
    if (desc < 0)
        return false;
    t1 = (-ray.dir.dot(ray.pt - pos) - sqrt(desc))/ray.dir.dot(ray.dir);
    t2 = (-ray.dir.dot(ray.pt - pos) + sqrt(desc))/ray.dir.dot(ray.dir);
    if (t1 < EPSILON) {
        if (t2 < EPSILON)
            return false;
        else
            t = t2;
    } else {
        if (t2 < EPSILON)
            t = t1;
        else
            t = t1 < t2 ? t1 : t2;
    }
    point = ray.pt + t*ray.dir;
    return true;
}

Triangle::Triangle(float3 p1, float3 p2, float3 p3)
{
    this->p1 = Point(p1.x, p1.y, p1.z);
    this->p2 = Point(p2.x, p2.y, p2.z);
    this->p3 = Point(p3.x, p3.y, p3.z);
    bbox.pMin = min(min(this->p1, this->p2), this->p3);
    bbox.pMax = max(max(this->p1, this->p2), this->p3);
}

Direct Triangle::getNormal(const Point& p) const
{
    return (p2 - p1).cross(p3 - p1).normalize();
}

bool Triangle::isHit(const Ray& ray, Point& point, float& t) const
{
    Plane plane(p1, p2, p3);
    Point pp;
    float tt;
    if (!plane.isHit(ray, pp, tt))
        return false;
    if (!isInside(pp))
        return false;
    point = pp;
    t = tt;
    return true;
}

static void __getCoefficient(const Point& p, const Point& p1,
        const Point& p2, const Point& p3, float& a, float& b, float& c)
{
    Direct norm = (p2 - p1).cross(p3 - p1);
    float area = norm.dot(norm);
    a = (p2 - p).cross(p3 - p).dot(norm) / area;
    b = (p3 - p).cross(p1 - p).dot(norm) / area;
    c = (p1 - p).cross(p2 - p).dot(norm) / area;
}

//assume that p is on the triangle plane
bool Triangle::isInside(const Point& p) const
{
    float a, b, c;
    __getCoefficient(p, p1, p2, p3, a, b, c);
    if (a < 0 || b < 0 || c < 0)
        return false;
    else
        return true;
}

NormTriangle::NormTriangle(const Vertex* v1,
        const Vertex* v2, const Vertex* v3)
{
    for (int i = 0; i < 3; i++) {
        p1[i] = v1->position[i];
        n1[i] = v1->normal[i];
        p2[i] = v2->position[i];
        n2[i] = v2->normal[i];
        p3[i] = v3->position[i];
        n3[i] = v3->normal[i];
    }
    bbox.pMin = min(min(this->p1, this->p2), this->p3);
    bbox.pMax = max(max(this->p1, this->p2), this->p3);
}

Direct NormTriangle::getNormal(const Point& p) const
{
    float a, b, c;
    __getCoefficient(p, p1, p2, p3, a, b, c);
    return -Direct(a*n1 + b*n2 + c*n3).normalize();
}

Plane::Plane(float3 n, float d)
{
    this->n = Direct(n.x, n.y, n.z).normalize();
    this->d = d;
}

Plane::Plane(float nx, float ny, float nz, float d)
{
    this->n = Direct(nx, ny, nz).normalize();
    this->d = d;
}

Plane::Plane(const Point& p1, const Point& p2, const Point& p3)
{
    n = (p2 - p1).cross(p3 - p1).normalize();
    d = -p1.dot(n);
}

Direct Plane::getNormal(const Point& p) const
{
    return n;
}

bool Plane::isHit(const Ray& ray, Point& point, float& t) const
{
    float tmp;
    float denom = ray.dir.dot(n);
    if (denom == 0)
        return false;
    tmp = -(ray.pt.dot(n) + d)/denom;
    if (tmp < EPSILON) {
        return false;
    } else {
        t = tmp;
        point = ray.pt + t*ray.dir;
        return true;
    }
}

PointLight::PointLight(float3 pos, float3 rgb)
{
    this->pos = Point(pos.x, pos.y, pos.z);
    this->rgb = Color(rgb.x, rgb.y, rgb.z);
}

bool PointLight::isReachable(const Point& p, const vector<Object*>& objects,
        const vector<Object*>& planes, const BvhTree& tree) const
{
    float tMin = FLT_MAX;
    vector<Object*> objs;
    vector<Object*>::const_iterator it;
    Ray ray(p, pos - p);
    if (tree.isValid())
        objs = tree.getPossibleObjs(ray);
    else
        objs = objects;
    for (it = objs.begin(); it != objs.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t) && tMin > t)
            tMin = t;
    }
    for (it = planes.begin(); it != planes.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t) && tMin > t) 
            tMin = t;
    }
    if (tMin < (pos - p).norm2())
        return false;
    return true;
}

Color PointLight::getColor(const Point& p, const Direct& n,
        const Direct& v, const Material& m,
        const vector<Object*>& objects, const vector<Object*>& planes,
        const BvhTree& tree) const
{
    Color c(0, 0, 0);
    if (isReachable(p, objects, planes, tree)) {
        Direct i = (pos - p).normalize();
        Direct h = (i + v).normalize();
        float cosd = i.dot(n);
        float coss = h.dot(n);
        c += m.diff * rgb * (cosd > 0 ? cosd : 0);
        c += m.spec * rgb * pow((coss > 0 ? coss : 0), m.r);
    }
    return c;
}

DirectionalLight::DirectionalLight(float3 dir, float3 rgb)
{
    this->dir = Direct(dir.x, dir.y, dir.z).normalize();
    this->rgb = Color(rgb.x, rgb.y, rgb.z);
}

bool DirectionalLight::isReachable(const Point& p,
        const vector<Object*>& objects, const vector<Object*>& planes,
        const BvhTree& tree) const
{
    vector<Object*> objs;
    vector<Object*>::const_iterator it;
    Ray ray(p, dir);
    if (tree.isValid())
        objs = tree.getPossibleObjs(ray);
    else
        objs = objects;
    for (it = objs.begin(); it != objs.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t))
            return false;
    }
    for (it = planes.begin(); it != planes.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t)) 
            return false;
    }
    return true;
}

Color DirectionalLight::getColor(const Point& p, const Direct& n,
        const Direct& v, const Material& m,
        const vector<Object*>& objects, const vector<Object*>& planes,
        const BvhTree& tree) const
{
    Color c(0, 0, 0);
    if (isReachable(p, objects, planes, tree)) {
        Direct h = (dir + v).normalize();
        float cosd = dir.dot(n);
        float coss = h.dot(n);
        c += m.diff * rgb * (cosd > 0 ? cosd : 0);
        c += m.spec * rgb * pow((coss > 0 ? coss : 0), m.r);
    }
    return c;
}

AmbientLight::AmbientLight(float3 rgb)
{
    this->rgb = Color(rgb.x, rgb.y, rgb.z);
}

Color AmbientLight::getColor(const Point& p, const Direct& n,
        const Direct& v, const Material& m,
        const vector<Object*>& objects, const vector<Object*>& planes,
        const BvhTree& tree) const
{
    return m.diff * rgb;
}

AreaLight::AreaLight(float3 pos, float3 rgb, int samp, float rad) :
    PointLight(pos, rgb)
{
    this->samp = samp;
    this->rad = rad;
}

bool AreaLight::isReachable(const Point& p,
        const std::vector<Object*>& objects,
        const std::vector<Object*>& planes, const BvhTree& tree,
        const Point& pp) const
{
    float tMin = FLT_MAX;
    vector<Object*> objs;
    vector<Object*>::const_iterator it;
    Ray ray(p, pp - p);
    if (tree.isValid())
        objs = tree.getPossibleObjs(ray);
    else
        objs = objects;
    for (it = objs.begin(); it != objs.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t) && tMin > t)
            tMin = t;
    }
    for (it = planes.begin(); it != planes.end(); it++) {
        float t;
        Point pt;
        if ((*it)->isHit(ray, pt, t) && tMin > t) 
            tMin = t;
    }
    if (tMin < (pp - p).norm2())
        return false;
    return true;
}

Color AreaLight::getColor(const Point& p, const Direct& n,
        const Direct& v, const Material& m,
        const vector<Object*>& objects, const vector<Object*>& planes,
        const BvhTree& tree) const
{
    Color c(0, 0, 0);
    for (int i = 0; i < samp; i++) {
        Direct noise(frand(), frand(), frand());
        Point pt(pos + noise*rad);
        if (isReachable(p, objects, planes, tree, pt)) {
            Direct i = (pt - p).normalize();
            Direct h = (i + v).normalize();
            float cosd = i.dot(n);
            float coss = h.dot(n);
            c += m.diff * rgb * (cosd > 0 ? cosd : 0);
            c += m.spec * rgb * pow((coss > 0 ? coss : 0), m.r);
        }
    }
    c /= samp;
    return c;
}

static bool __comp(const Object* o1, const Object* o2)
{
    return o1->key < o2->key;
}

static BoundingBox __getBbox(const vector<Object*>& objs)
{
    BoundingBox bbox;
    vector<Object*>::const_iterator it;
    bbox.pMin = FLT_MAX;
    bbox.pMax = -FLT_MAX;
    for (it = objs.begin(); it != objs.end(); it++) {
        bbox.pMin = min(bbox.pMin, (*it)->bbox.pMin);
        bbox.pMax = max(bbox.pMax, (*it)->bbox.pMax);
    }
    return bbox;
}

static vector<Object*> __split(vector<Object*>& left)
{
    vector<Object*> right;
    int size = left.size();
    for (int i = 0; i < size/2; i++) {
        right.push_back(left.back());
        left.pop_back();
    }
    reverse(right.begin(), right.end());
    return right;
}

static TreeNode* __build(vector<Object*>& objs)
{
    TreeNode* node = 0;
    if (objs.empty())
        return 0;
    node = new TreeNode();
    node->bbox = __getBbox(objs);
    if (objs.size() == 1) {
        node->obj = objs.front();
    } else {
        vector<Object*> right = __split(objs);
        node->left = __build(objs);
        node->right = __build(right);
    }
    return node;
}

vector<Object*> BvhTree::buildTree(const vector<Object*>& objects,
        const BoundingBox& bbox)
{
    int idx;
    Direct dif = bbox.pMax - bbox.pMin;
    vector<Object*> boundedObjs;
    vector<Object*> planes;
    vector<Object*>::const_iterator it;

    valid = true;
    if (dif[0] > dif[1] && dif[0] > dif[2])
        idx = 0;
    else if (dif[1] > dif[0] && dif[1] > dif[2])
        idx = 1;
    else
        idx = 2;

    for (it = objects.begin(); it != objects.end(); it++) {
        if ((*it)->isBounded()) {
            (*it)->key = (*it)->bbox.pMin[idx];
            boundedObjs.push_back(*it);
        } else {
            planes.push_back(*it);
        }
    }

    sort(boundedObjs.begin(), boundedObjs.end(), __comp);
    root = __build(boundedObjs);
    return planes;
}

void BvhTree::freeTree(TreeNode* node)
{
    if (!node)
        return;
    if (node->left)
        freeTree(node->left);
    if (node->right)
        freeTree(node->right);
    delete node;
    node = 0;
}

vector<Object*> BvhTree::getPossibleObjs(const Ray& ray) const
{
    vector<Object*> objs;
    getObjs(root, ray, objs);
    return objs;
}

void BvhTree::getObjs(const TreeNode* node, const Ray& ray,
        vector<Object*>& objs) const
{
    if (!node->bbox.isHit(ray))
        return;
    if (node->obj) {
        objs.push_back(node->obj);
    } else {
        getObjs(node->left, ray, objs);
        getObjs(node->right, ray, objs);
    }
}

float frand()
{
    return 1.0*rand()/RAND_MAX - 0.5;
}
