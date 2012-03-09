all: process

CC = g++
INCLUDE = -I/usr/include/opencv
CFLAGS = -O2 -Wall
LIBS = -lhighgui -ltbb -lcxcore

process: main.o parse.o raytracer.o scene.o mesh.o
	$(CC) $(INCLUDE) $(CFLAGS) -o process main.o parse.o raytracer.o scene.o mesh.o  $(LIBS)

main.o: main.cpp raytracer.h
	$(CC) $(INCLUDE) $(CFLAGS) -c main.cpp

parse.o: parse.cpp parse.h
	$(CC) $(INCLUDE) $(CFLAGS) -c parse.cpp

raytracer.o: raytracer.cpp raytracer.h
	$(CC) $(INCLUDE) $(CFLAGS) -c raytracer.cpp

scene.o: scene.cpp scene.h
	$(CC) $(INCLUDE) $(CFLAGS) -c scene.cpp

mesh.o: mesh.cpp mesh.h vector3.h
	$(CC) $(INCLUDE) $(CFLAGS) -c mesh.cpp

.PHONY: clean

clean:
	rm -f *.o process
