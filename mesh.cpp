#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "mesh.h"
#include "vector3.h"
using namespace std;

int countF = 0;

HEdge* HEdge::prev()
{
    HEdge* curr = this;
    HEdge* prev = 0;
    do {
        prev = curr;
        curr = curr->next;
    } while (curr != this);
    return prev;
}

HEdge* Mesh::findHEdge(Vertex* start, Vertex* end) const
{
    HEdge* curr = start->halfedge;
    if (!curr)
        return 0;
    do {
        if (curr->pair->vertex == end)
            return curr;
        curr = curr->pair->next;
    } while (curr != start->halfedge);
    return 0;
}

HEdge* Mesh::findFreeIncident(Vertex* v) const
{
    HEdge* curr = v->halfedge;
    if (!curr)
        return 0;
    do {
        if (!curr->pair->face)
            return curr->pair;
        curr = curr->pair->next;
    } while (curr != v->halfedge);
    return 0;
}

HEdge* Mesh::findFreeIncident(HEdge* start, HEdge* end) const
{
    HEdge* curr = start;
    if (end == start)
        return 0;
    do {
        if (!curr->pair->face)
            return curr->pair;
        curr = curr->pair->next;
    } while (curr != end);
    return 0;
}

bool Mesh::makeAdj(HEdge* in, HEdge* out)
{
    if (in->next == out)
        return true;

    HEdge* b = in->next;
    HEdge* d = out->prev();
    HEdge* g = findFreeIncident(out, in->pair);
    if (!g)
        return false;
    HEdge* h = g->next;

    in->next = out;
    g->next = b;
    d->next = h;

    return true;
}

HEdge* Mesh::addHEdge(Vertex* start, Vertex* end)
{
    HEdge* he = new HEdge();
    HEdge* bhe = new HEdge();
    
    he->next = bhe;
    he->pair = bhe;
    he->vertex = start;

    bhe->next = he;
    bhe->pair = he;
    bhe->vertex = end;

    if (!start->halfedge) {
        start->halfedge = he;
    } else {
        HEdge* in = findFreeIncident(start);
        HEdge* out = in->next;

        in->next = he;
        bhe->next = out;
    }

    if (!end->halfedge) {
        end->halfedge = bhe;
    } else {
        HEdge* in = findFreeIncident(end);
        HEdge* out = in->next;

        in->next = bhe;
        he->next = out;
    }

    helist.push_back(he);
    helist.push_back(bhe);

    return he;
}

void Mesh::addFace(int v0, int v1, int v2)
{
    Face* f = new Face();
    Vertex* v[3];
    HEdge* he[3];
    v[0] = vlist[v0];
    v[1] = vlist[v1];
    v[2] = vlist[v2];

    for (int i = 0; i < 3; i++) {
        he[i] = findHEdge(v[i], v[(i+1)%3]);
        if (!he[i])
            he[i] = addHEdge(v[i], v[(i+1)%3]);
    }
    for (int i = 0; i < 3; i++)
        makeAdj(he[i], he[(i+1)%3]);

    f->halfedge = he[0];
    f->index = countF;
    countF++;

    for (int i = 0; i < 3; i++)
        he[i]->face = f;

    flist.push_back(f);
}

bool Mesh::readOBJ(const char* filename)
{
    if (!filename || strlen(filename) == 0) {
        cerr << "ERROR: No such file" << endl;
        return false;
    }
    ifstream file(filename);
    if (!file) {
        cerr << "ERROR: No such file" << endl;
        return false;
    }

    int countV = 0;
    int countVN = 0;
    char buf[1024];
    while (file.getline(buf, 1024)) { 
        char type[16];
        istringstream line(buf);
        line >> type;

        if (!strcmp(type, "v")) {
            double x, y, z;
            Vertex* v;
            line >> x >> y >> z;
            v = new Vertex(x, y, z);
            v->index = countV;
            addVertex(v);
            countV++;
        } else if (!strcmp(type, "vn")) {
            double i, j, k;
            line >> i >> j >> k;
            nlist.push_back(Vector3d(i, j, k));
            countVN++;
        } else if (!strcmp(type, "f")) {
            int index[3][3];
            string str[3];
            stringstream ss[3];
            string s[3][3];
            line >> str[0] >> str[1];
            do {
                line >> str[2];
                if(str[1] == str[2])
                    break;
                for (int i = 0; i < 3; i++) {
                    int j = 0;
                    ss[i] << str[i];
                    while (getline(ss[i], s[i][j], '/'))
                        j++;
                    index[i][0] = atoi(s[i][0].c_str()) - 1;
                }
                addFace(index[0][0], index[1][0], index[2][0]);
                for (int i = 0; i < 3; i++) {
                    if (!s[i][2].empty()) {
                        index[i][2] = atoi(s[i][2].c_str()) - 1;
                        vlist[index[i][0]]->normal = nlist[index[i][2]];
                        vlist[index[i][0]]->normalized = true;
                    }
                }
                str[1] = str[2];
            } while (1);
        }
    }
    file.close();

    for (int i = 0; i < (int)vlist.size(); i++) {
        if (!vlist[i]->normalized) {
            Vertex* v0 = vlist[i];
            Vertex* v1;
            Vertex* v2;
            Vector3d n1;
            Vector3d n2;
            Vector3d norm;
            HEdge* start = v0->halfedge;
            HEdge* curr = start;
            if (!start) {
                v0->normalized = true;
                continue;
            }
            do {
                v1 = curr->pair->vertex;
                curr = curr->pair->next;
                v2 = curr->pair->vertex;
                n1 = v1->position - v0->position;
                n2 = v2->position - v0->position;
                norm += (n1.cross(n2)).normalize();
            } while (curr != start);
            v0->normal = norm.normalize();
            v0->normalized = true;
        }
    }

    return true;
}

void Mesh::getVFromV(int i) const
{
    ostringstream tmp;
    int count = 0;
    HEdge* start = vlist[i]->halfedge;
    if (!start) {
        cout << i + 1 << endl;
        cout << 0 << endl;
    } else {
        HEdge* curr = start;
        do {
            tmp << curr->pair->vertex->index + 1 << ' ';
            count++;
            curr = curr->pair->next;
        } while (curr != start);
        cout << i + 1 << endl;
        cout << count << endl;
        cout << tmp.str() << endl;
    }
}

void Mesh::getVFromF(int i) const
{
    HEdge* start = flist[i]->halfedge;
    HEdge* curr = start;
    cout << i + 1 << endl;
    do {
        cout << curr->vertex->index + 1 << ' ';
        curr = curr->next;
    } while (curr != start);
    cout << endl;
}

void Mesh::getFFromF(int i) const
{
    HEdge* start = flist[i]->halfedge;
    HEdge* curr = start;
    cout << i + 1 << endl;
    do {
        cout << curr->pair->face->index + 1 << ' ';
        curr = curr->next;
    } while (curr!= start);
    cout << endl;
}
