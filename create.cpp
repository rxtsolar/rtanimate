#include <iostream>

#include "physics.h"
using namespace std;
using namespace phy;

int main(int argc, char** argv)
{
	Physics physics;
	if (argc != 3 && argc != 2) {
		cerr << "usage: " << argv[0] <<
			" <scene.sce> [output_suffix]" << endl;
		return 1;
	}
	physics.parse(argv[1]);
	physics.write(argv[2] ? argv[2] : "scene");
	return 0;
}
