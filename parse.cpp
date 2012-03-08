#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "parse.h"

using namespace std;

void Parser::parse(const char *file) {
    ifstream in(file);
    char buffer[1025];
    string cmd;

    for (int line = 1; in.good(); line++) {
        in.getline(buffer, 1024);
        buffer[in.gcount()] = 0;

        cmd = "";

        istringstream iss(buffer);

        iss >> cmd;
        if (cmd[0] == '/' || cmd.empty()) {
            continue;
        } else if (cmd == "s") {
            float3 pos;
            float r;
            iss >> pos >> r;
            sphere(pos, r);
        } else if (cmd == "t") {
            float3 a, b, c;
            iss >> a >> b >> c;
            triangle(a, b, c);
        } else if (cmd == "p") {
            float3 n;
            float d;
            iss >> n >> d;
            plane(n, d);
        } else if (cmd == "c") {
            float3 pos, dir;
            float d, iw, ih;
            int pw, ph;
            iss >> pos >> dir >> d >> iw >> ih >> pw >> ph;
            camera(pos, dir, d, iw, ih, pw, ph);
        } else if (cmd == "l") {
            iss >> cmd;
            if (cmd == "p") {
                float3 pos, rgb;
                iss >> pos >> rgb;
                pointLight(pos, rgb);
            } else if (cmd == "d") {
                float3 dir, rgb;
                iss >> dir >> rgb;
                directionalLight(dir, rgb);
            } else if (cmd == "a") {
                float3 rgb;
                iss >> rgb;
                ambientLight(rgb);
            } else if (cmd == "r") {
                float3 pos, rgb;
                int samp;
                float rad;
                iss >> pos >> rgb >> samp >> rad;
                areaLight(pos, rgb, samp, rad);
            } else {
                cout << "Parser error: invalid light at line " << line << endl;
            }
        } else if (cmd == "m") {
            float3 diff, spec, refl;
            float r;
            iss >> diff >> spec >> r >> refl;
            material(diff, spec, r, refl);
        } else if (cmd == "obj") {
            Mesh mesh;
            string name;
            FaceList::iterator it;
            iss >> name;
            if (mesh.readOBJ(name.c_str())) {
                for (it = mesh.flist.begin(); it != mesh.flist.end(); it++) {
                    HEdge* he = (*it)->halfedge;
                    normtriangle(he->vertex, he->next->vertex,
                            he->next->next->vertex);
                }
            }
        } else if (cmd == "o") {
            string option;
            while (iss.good()) {
                iss >> option;
                if (option == "r") {
                    int n;
                    iss >> n;
                    setRecursion(n);
                } else if (option == "a") {
                    int n;
                    iss >> n;
                    setSample(n);
                } else if (option == "b") {
                    setBVH();
                } else if (option == "m") {
                    setTBB();
                } else {
                    cerr << "Option error: invalid option" << endl;
                }
            }
        } else {
            cerr << "Parser error: invalid command at line " << line << endl;
        }
    }

    in.close();
}
