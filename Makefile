all:
	g++ -Wall main.cpp parse.cpp raytracer.cpp scene.cpp mesh.cpp -o main_t06m01 -O2 `pkg-config --libs --cflags opencv tbb`
