all: render create
#rm *.o

CC = g++
INCLUDE = -I/usr/include/opencv
CFLAGS = -g -Wall -c
LFLAGS = -o
LIBS = -lhighgui -ltbb -lcxcore

render: main.o parse.o raytracer.o scene.o mesh.o
	$(CC) $(INCLUDE) $(LFLAGS) process main.o parse.o raytracer.o scene.o mesh.o $(LIBS)

create: create.o physics.o parse.o mesh.o
	$(CC) $(LFLAGS) create create.o physics.o parse.o mesh.o

parse.h: mesh.h
raytracer.h: mesh.h scene.h parse.h
scene.h: vector3.h parse.h
mesh.h: vector3.h
physics.h: vector3.h parse.h

main.o: main.cpp raytracer.h
	$(CC) $(INCLUDE) $(CFLAGS) main.cpp

parse.o: parse.cpp parse.h
	$(CC) $(INCLUDE) $(CFLAGS) parse.cpp

raytracer.o: raytracer.cpp raytracer.h
	$(CC) $(INCLUDE) $(CFLAGS) raytracer.cpp

scene.o: scene.cpp scene.h
	$(CC) $(INCLUDE) $(CFLAGS) scene.cpp

mesh.o: mesh.cpp mesh.h vector3.h
	$(CC) $(INCLUDE) $(CFLAGS) mesh.cpp

create.o: create.cpp physics.h
	$(CC) $(INCLUDE) $(CFLAGS) create.cpp

physics.o: physics.cpp physics.h
	$(CC) $(INCLUDE) $(CFLAGS) physics.cpp

.PHONY: clean
clean:
	rm -f *.o process create
