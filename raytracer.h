#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <highgui.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "mesh.h"
#include "scene.h"
#include "parse.h"

class Render;
class RayTracer;

class RayTracer : public Parser {
public:
	RayTracer() : recur(3), background(0, 0, 0), useTBB(false),
	useBVH(false), antialiasing(false) {}
	BoundingBox bbox;

	void write(const char*);

	virtual ~RayTracer();
	friend class Render;

protected:
	int samp;
	int recur;
	const Color background;
	cv::Mat image;
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	Camera cam;
	Material mat;
	BvhTree tree;
	std::vector<Object*> planes;

	Point center;
	Point upperLeft;
	Direct up;
	Direct right;
	Direct down;

	bool useTBB;
	bool useBVH;
	bool antialiasing;

	void render();
	Color getColor(const Ray&, int) const;
	void getPixel(int i, int j);

	virtual void sphere(float3, float);
	virtual void triangle(float3, float3, float3);
	virtual void normtriangle(const Vertex*,
			const Vertex*, const Vertex*);
	virtual void plane(float3, float); 
	virtual void camera(float3, float3,
			float, float, float, int, int);
	virtual void pointLight(float3, float3); 
	virtual void areaLight(float3, float3, int, float); 
	virtual void directionalLight(float3, float3);
	virtual void ambientLight(float3); 
	virtual void material(float3, float3, float, float3);
	virtual void setRecursion(int n) { recur = n; }
	virtual void setSample(int s)
	{ samp = s; antialiasing = true; }
	virtual void setBVH() { useBVH = true; }
	virtual void setTBB() { useTBB = true; }
};

class Render {
public:
	Render(RayTracer* const r) { rt = r; }
	void operator()(const tbb::blocked_range<int>&) const;
private:
	RayTracer* rt;
};


#endif
