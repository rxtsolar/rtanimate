#include <cfloat>
#include <vector>

#include "raytracer.h"

using namespace std;
using namespace tbb;

extern float EPSILON;

RayTracer::~RayTracer()
{
	vector<Object*>::iterator oIt;
	vector<Light*>::iterator lIt;
	for (oIt = objects.begin(); oIt != objects.end(); oIt++)
		delete *oIt;
	for (lIt = lights.begin(); lIt != lights.end(); lIt++)
		delete *lIt;
	objects.clear();
	lights.clear();
	planes.clear();
}

void RayTracer::write(const char* output)
{
	if (useBVH)
		planes = tree.buildTree(objects, bbox);
	render();
	cv::imwrite(output, image);
}

void RayTracer::sphere(float3 pos, float r)
{
	Object* obj = new Sphere(pos, r);
	obj->mat = mat;
	objects.push_back(obj);
	bbox.pMin = min(bbox.pMin, obj->bbox.pMin);
	bbox.pMax = max(bbox.pMax, obj->bbox.pMax);
}

void RayTracer::triangle(float3 p1, float3 p2, float3 p3)
{
	Object* obj = new Triangle(p1, p2, p3);
	obj->mat = mat;
	objects.push_back(obj);
	bbox.pMin = min(bbox.pMin, obj->bbox.pMin);
	bbox.pMax = max(bbox.pMax, obj->bbox.pMax);
}

void RayTracer::normtriangle(const Vertex* v1,
		const Vertex* v2, const Vertex* v3)
{
	Object* obj = new NormTriangle(v1, v2, v3);
	obj->mat = mat;
	objects.push_back(obj);
	bbox.pMin = min(bbox.pMin, obj->bbox.pMin);
	bbox.pMax = max(bbox.pMax, obj->bbox.pMax);
}

void RayTracer::plane(float3 n, float d)
{
	Object* obj = new Plane(n, d);
	obj->mat = mat;
	objects.push_back(obj);
}

void RayTracer::camera(float3 pos, float3 dir,
		float d, float iw, float ih, int pw, int ph)
{
	cam.pos = Point(pos.x, pos.y, pos.z);
	cam.dir = Direct(dir.x, dir.y, dir.z).normalize();
	cam.d = d;
	cam.iw = iw;
	cam.ih = ih;
	cam.pw = pw;
	cam.ph = ph;
}

void RayTracer::pointLight(float3 pos, float3 rgb)
{
	Light* light = new PointLight(pos, rgb);
	lights.push_back(light);
}

void RayTracer::areaLight(float3 pos, float3 rgb, int samp, float rad)
{
	Light* light = new AreaLight(pos, rgb, samp, rad);
	lights.push_back(light);
}

void RayTracer::directionalLight(float3 dir, float3 rgb)
{
	Light* light = new DirectionalLight(dir, rgb);
	lights.push_back(light);
}

void RayTracer::ambientLight(float3 rgb)
{
	Light* light = new AmbientLight(rgb);
	lights.push_back(light);
}

void RayTracer::material(float3 diff, float3 spec, float r, float3 refl)
{
	mat.diff = Color(diff.x, diff.y, diff.z);
	mat.spec = Color(spec.x, spec.y, spec.z);
	mat.r = r;
	mat.refl = Color(refl.x, refl.y, refl.z);
}

static Object* __getIntersection(const Ray& ray, const vector<Object*>& objs,
		const vector<Object*>& planes, Point& p)
{
	float tMin = FLT_MAX;
	vector<Object*>::const_iterator it;
	Object* o = 0;
	for (it = objs.begin(); it != objs.end(); it++) {
		float t;
		Point pt;
		if ((*it)->isHit(ray, pt, t) && tMin > t) {
			tMin = t;
			p = pt;
			o = *it;
		}
	}
	for (it = planes.begin(); it != planes.end(); it++) {
		float t;
		Point pt;
		if ((*it)->isHit(ray, pt, t) && tMin > t) {
			tMin = t;
			p = pt;
			o = *it;
		}
	}
	return o;
}

void Render::operator()(const blocked_range<int>& r) const
{
	for (int j = r.begin(); j != r.end(); j++) {
		for (int i = 0; i < rt->cam.pw; i++) {
			rt->getPixel(i, j);
		}
	}
}

void RayTracer::render()
{
	EPSILON = (bbox.pMax - bbox.pMin).norm2() / 10000;
	center = Point(cam.pos + cam.d * cam.dir);
	up = Direct(0.0, 0.1, 0.0);
	right = Direct(cam.dir.cross(up).normalize());
	down = Direct(cam.dir.cross(right).normalize());
	upperLeft = Point(center - cam.iw/2*right - cam.ih/2*down);
	right *= cam.iw/cam.pw;
	down *= cam.ih/cam.ph;

	srand(time(0));
	image = cv::Mat(cam.ph, cam.pw, CV_64FC3);

	if (useTBB) {
		parallel_for(blocked_range<int>(0, cam.ph),
				Render(this));
	} else {
		for (int j = 0; j < cam.ph; j++) {
			for (int i = 0; i < cam.pw; i++) {
				getPixel(i, j);
			}
		}
	}
}

void RayTracer::getPixel(int i, int j)
{
	double r;
	double g;
	double b;
	if (antialiasing) {
		float zero = 0.5/samp;
		float step = 1.0/samp;
		r = 0;
		g = 0;
		b = 0;
		for (int jj = 0; jj < samp; jj++) {
			float y = zero + (jj + frand())*step;
			for (int ii = 0; ii < samp; ii++) {
				float x = zero + (ii + frand())*step;
				Point iPt = upperLeft + (i + x)*right + (j + y)*down;
				Ray ray(cam.pos, iPt - cam.pos);
				Color c = getColor(ray, 0);
				r += c[0];
				g += c[1];
				b += c[2];
			}
		}
		r /= samp*samp;
		g /= samp*samp;
		b /= samp*samp;
	} else {
		Point iPt = upperLeft + (i + 0.5)*right + (j + 0.5)*down;
		Ray ray(cam.pos, iPt - cam.pos);
		Color c = getColor(ray, 0);
		r = c[0];
		g = c[1];
		b = c[2];
	}
	image.at<cv::Vec3d>(j, i)[0] = b*255;
	image.at<cv::Vec3d>(j, i)[1] = g*255;
	image.at<cv::Vec3d>(j, i)[2] = r*255;
	cerr << i << ", " << j << endl;
}

Color RayTracer::getColor(const Ray& ray, int rec) const
{
	Color c = background;
	vector<Object*> objs;
	vector<Light*>::const_iterator it;
	Point p;
	Object* o = 0;
	Direct n;

	if (useBVH)
		objs = tree.getPossibleObjs(ray);
	else
		objs = objects;
	o = __getIntersection(ray, objs, planes, p);
	if (!o) {
		return c;
	}
	n = o->getNormal(p);

	for (it = lights.begin(); it != lights.end(); it++) {
		c += (*it)->getColor(p, n, -ray.dir, o->mat, objects, planes, tree);
	}
	if (rec < recur) {
		rec++;
		Ray r = Ray(p, ray.dir - 2*ray.dir.dot(n)*n);
		if (o->mat.refl != Color(0, 0, 0))
			c += o->mat.refl * getColor(r, rec);
	}

	return c;
}
