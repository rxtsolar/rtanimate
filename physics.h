#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "vector3.h"
#include "parse.h"

namespace phy {

struct Camera;
struct Light;
struct Material;
struct Sphere;
struct Plane;
class Physics;

struct Camera {
	void move(float dt);
	Vector3f pos;
	Vector3f dir;
	Vector3f velo;
	Vector3f acce;
	float d;
	float iw;
	float ih;
	int pw;
	int ph;
};

struct Light {
	void move(float dt);
	Vector3f pos;
	Vector3f rgb;
	Vector3f velo;
	Vector3f acce;
};

struct Material {
	Vector3f diff;
	Vector3f spec;
	float r;
	Vector3f refl;
	Material& operator=(const Material&);
};

struct Sphere {
	void move(float dt);
	void collide(vector<Sphere>& spheres,
			const vector<Plane>& planes);
	void checkAndChange(Sphere& sphere);
	void checkAndChange(const Plane& plane);
	Vector3f pos;
	Vector3f velo;
	Vector3f acce;
	float r;
	float e;
	float mu;
	float mass;
	Material mat;
};

struct Plane {
	Vector3f n;
	float d;
	float e;
	float mu;
	Material mat;
};

class Physics : public Parser {
public:
	virtual void write(const char* name);

protected:
	virtual void move();
	virtual void material(float3 diff, float3 spec, float r, float3 refl);
	virtual void myCamera(float3 pos, float3 dir, float3 velo, float3 acce,
			float d, float iw, float ih, int pw, int ph);
	virtual void myLight(float3 pos, float3 rgb, float3 velo, float3 acce);
	virtual void mySphere(float3 pos, float r, float3 velo, float3 acce,
			float e, float mu, float mass);
	virtual void myPlane(float3 n, float d, float e, float mu);
	virtual void setFrames(int f) { frames = f; }
	virtual void setTimeStep(float t) { dt = t; }

	int frames;
	float dt;
	vector<Sphere> spheres;
	vector<Plane> planes;
	vector<Light> lights;
	Material mat;
	Camera cam;
};

} //end namespace phy

#endif
