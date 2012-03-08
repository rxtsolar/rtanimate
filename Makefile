all:
	g++ -I. -I/usr/include/OpenEXR -L/usr/local/lib -ltbb -lIlmImf -lImath -lHalf -Wall main.cpp parse.cpp raytracer.cpp scene.cpp mesh.cpp -o main_t06m01 -O2 `pkg-config --libs --cflags opencv tbb`
