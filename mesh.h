#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "vector3.h"

class HEdge;
class Face;
class Vertex;
typedef std::vector<HEdge*> HEdgeList;
typedef std::vector<Face*> FaceList;
typedef std::vector<Vertex*> VertexList;
typedef std::vector<Vector3d> NormalList;

class HEdge {
public:
    HEdge* next;
    HEdge* pair;
    Vertex* vertex;
    Face* face;

    HEdge()
    {
        pair = 0;
        next = 0;
        vertex = 0;
        face = 0;
    }
    HEdge* prev();
};

class Face {
public:
    HEdge* halfedge;
    int index;

    Face()
    {
        halfedge = 0;
    }
};

class Vertex {
public:
    Vector3d position;
    Vector3d normal;
    Vector3d color;
    HEdge* halfedge;
    int index;
    bool normalized;

    Vertex()
    {
        halfedge = 0;
        normalized = false;
    }
    
    Vertex(const Vector3d& vect)
    {
        halfedge = 0;
        normalized = false;
        position = vect;
    }

    Vertex(double x, double y, double z)
    {
        halfedge = 0;
        normalized = false;
        position[0] = x;
        position[1] = y;
        position[2] = z;
    }
};

class Mesh {
public:
    HEdgeList helist;
    FaceList flist;
    VertexList vlist;
    NormalList nlist;
    Vector3d minV;
    Vector3d maxV;

    ~Mesh()
    {
        for (int i = 0; i < (int)helist.size(); i++)
            delete helist[i];
        for (int i = 0; i < (int)flist.size(); i++)
            delete flist[i];
        for (int i = 0; i < (int)vlist.size(); i++)
            delete vlist[i];
        helist.clear();
        vlist.clear();
        flist.clear();
    }

    void addVertex(Vertex* v)
    {
        if (vlist.empty()) {
            maxV = minV = v->position;
        } else {
            maxV = max(maxV, v->position);
            minV = min(minV, v->position);
        }
        vlist.push_back(v);
    }

    HEdge* findFreeIncident(Vertex* v) const;
    HEdge* findFreeIncident(HEdge* start, HEdge* end) const;
    HEdge* findHEdge(Vertex* start, Vertex* end) const;
    bool makeAdj(HEdge* in, HEdge* out);
    HEdge* addHEdge(Vertex* start, Vertex* end);
    void addFace(int v0, int v1, int v2);
    bool readOBJ(const char* filename);
    void getVFromV(int i) const;
    void getVFromF(int i) const;
    void getFFromF(int i) const;
};

#endif
