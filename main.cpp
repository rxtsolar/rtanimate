#include <iostream>
using namespace std;

#include "raytracer.h"

int main(int argc, char** argv)
{
	RayTracer rt;
	if (argc != 3 && argc !=2) {
		cerr << "usage: " << argv[0] <<
			" <scene.txt> [output.png]" << endl;
		return 1;
	}
	rt.parse(argv[1]);
	rt.write(argv[2] ? argv[2] : "output.png");
	return 0;
}
