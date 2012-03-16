#include <iostream>
#include "model.h"

using namespace std;
using namespace model;

int main(int argc, char** argv)
{
	Model m;
	if (argc != 3 && argc !=2) {
		cerr << "usage: " << argv[0] <<
			" <image.xxx> [output.sce]" << endl;
		return 1;
	}
	m.load(argv[1]);
	m.write(argv[2] ? argv[2] : "output.sce");
	return 0;
}
