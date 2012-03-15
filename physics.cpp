#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "physics.h"

using namespace phy;
using namespace std;

Material& Material::operator=(const Material& m)
{
	diff = m.diff;
	spec = m.spec;
	r = m.r;
	refl = m.refl;
	return *this;
}

void Physics::myCamera(float3 pos, float3 dir, float3 velo,
	float3 acce, float d, float iw, float ih, int pw, int ph)
{
	cam.pos = Vector3f(pos.x, pos.y, pos.z);
	cam.dir = Vector3f(dir.x, dir.y, dir.z);
	cam.velo = Vector3f(velo.x, velo.y, velo.z);
	cam.acce = Vector3f(acce.x, acce.y, acce.z);
	cam.d = d;
	cam.iw = iw;
	cam.ih = ih;
	cam.pw = pw;
	cam.ph = ph;
}

void Physics::myLight(float3 pos, float3 rgb, float3 velo, float3 acce)
{
	Light light;
	light.pos = Vector3f(pos.x, pos.y, pos.z);
	light.rgb = Vector3f(rgb.x, rgb.y, rgb.z);
	light.velo = Vector3f(velo.x, velo.y, velo.z);
	light.acce = Vector3f(acce.x, acce.y, acce.z);
	lights.push_back(light);
}

void Physics::mySphere(float3 pos, float r, float3 velo,
	float3 acce, float e, float mu, float mass)
{
	Sphere sphere;
	sphere.pos = Vector3f(pos.x, pos.y, pos.z);
	sphere.r = r;
	sphere.velo = Vector3f(velo.x, velo.y, velo.z);
	sphere.acce = Vector3f(acce.x, acce.y, acce.z);
	sphere.e = e;
	sphere.mu = mu;
	sphere.mass = mass;
	sphere.mat = mat;
	spheres.push_back(sphere);
}

void Physics::myPlane(float3 n, float d, float e, float mu)
{
	Plane plane;
	plane.n = Vector3f(n.x, n.y, n.z);
	plane.d = d;
	plane.e = e;
	plane.mu = mu;
	plane.mat = mat;
	planes.push_back(plane);
}

void Physics::material(float3 diff, float3 spec, float r, float3 refl)
{
	mat.diff = Vector3f(diff.x, diff.y, diff.z);
	mat.spec = Vector3f(spec.x, spec.y, spec.z);
	mat.r = r;
	mat.refl = Vector3f(refl.x, refl.y, refl.z);
}

void Physics::write(const char* name)
{
	for (int i = 0; i < frames; i++) {
		stringstream ss;
		ss << name << '_' << i << ".rnd";
		ofstream file(ss.str().c_str());
		file << "/ this is frame " << i << " of " << name << endl;
		// camera
		file << "/ camera" << endl;
		file << "c ";
		file << cam.pos.x() << ' ';
		file << cam.pos.y() << ' ';
		file << cam.pos.z() << ' ';
		file << cam.dir.x() << ' ';
		file << cam.dir.y() << ' ';
		file << cam.dir.z() << ' ';
		file << cam.d << ' ' << cam.iw << ' ' << cam.ih << ' ';
		file << cam.pw << ' ' << cam.ph << endl << endl;
		// lights
		file << "/ lights" << endl;
		for (vector<Light>::const_iterator itl = lights.begin();
				itl != lights.end(); itl++) {
			file << "l p ";
			file << itl->pos.x() << ' ';
			file << itl->pos.y() << ' ';
			file << itl->pos.z() << ' ';
			file << itl->rgb.x() << ' ';
			file << itl->rgb.y() << ' ';
			file << itl->rgb.z() << endl;
		}
		file << endl;
		// spheres
		file << "/ spheres" << endl;
		for (vector<Sphere>::const_iterator its = spheres.begin();
				its != spheres.end(); its++) {
			file << "m ";
			file << its->mat.diff.x() << ' ';
			file << its->mat.diff.y() << ' ';
			file << its->mat.diff.z() << ' ';
			file << its->mat.spec.x() << ' ';
			file << its->mat.spec.y() << ' ';
			file << its->mat.spec.z() << ' ';
			file << its->mat.r << ' ';
			file << its->mat.refl.x() << ' ';
			file << its->mat.refl.y() << ' ';
			file << its->mat.refl.z() << endl;
			file << "s ";
			file << its->pos.x() << ' ';
			file << its->pos.y() << ' ';
			file << its->pos.z() << ' ';
			file << its->r << endl;
		}
		file << endl;
		// planes
		file << "/ planes" << endl;
		for (vector<Plane>::const_iterator itp = planes.begin();
				itp != planes.end(); itp++) {
			file << "m ";
			file << itp->mat.diff.x() << ' ';
			file << itp->mat.diff.y() << ' ';
			file << itp->mat.diff.z() << ' ';
			file << itp->mat.spec.x() << ' ';
			file << itp->mat.spec.y() << ' ';
			file << itp->mat.spec.z() << ' ';
			file << itp->mat.r << ' ';
			file << itp->mat.refl.x() << ' ';
			file << itp->mat.refl.y() << ' ';
			file << itp->mat.refl.z() << endl;
			file << "p ";
			file << itp->n.x() << ' ';
			file << itp->n.y() << ' ';
			file << itp->n.z() << ' ';
			file << itp->d << endl;
		}
		file << endl;

		move();
	}
}

void Physics::move()
{
	cam.move(dt);
	for (vector<Light>::iterator itl = lights.begin();
			itl != lights.end(); itl++)
		itl->move(dt);

	for (vector<Sphere>::iterator its = spheres.begin();
			its != spheres.end(); its++)
		its->move(dt);

	for (vector<Sphere>::iterator its = spheres.begin();
			its != spheres.end(); its++)
		its->collide(spheres, planes);
}

void Camera::move(float dt)
{
	pos += velo * dt;
	velo += acce * dt;
}

void Light::move(float dt)
{
	pos += velo * dt;
	velo += acce * dt;
}

void Sphere::move(float dt)
{
	pos += velo * dt;
	velo += acce * dt;
}

void Sphere::checkAndChange(Sphere& s)
{
	if (this == &s)
		return;

	Vector3f dist = s.pos - pos;
	if (dist.dot(dist) > (r + s.r)*(r + s.r))
		return;
	if (s.velo.dot(dist) > velo.dot(dist))
		return;

	dist = dist.normalize();
	Vector3f vn1 = velo.dot(dist) * dist;
	Vector3f vn2 = s.velo.dot(dist) * dist;
	Vector3f vt1 = velo - vn1;
	Vector3f vt2 = s.velo - vn2;
	float ee = e * s.e;

	Vector3f vvn1 = (ee*mass - s.mass)*vn1 + (ee + 1)*s.mass*vn2;
	vvn1 /= ee*(mass + s.mass);
	Vector3f vvn2 = (ee + 1)*mass*vn1 + (ee*s.mass - mass)*vn2;
	vvn2 /= ee*(mass + s.mass);

	velo = vvn1 + vt1;
	s.velo = vvn2 + vt2;
}

void Sphere::checkAndChange(const Plane& p)
{
	if (pos.dot(p.n) + p.d > r)
		return;

	if (velo.dot(p.n) > 0)
		return;

	Vector3f vn = velo.dot(p.n) * p.n;
	Vector3f vt = velo - vn;
	float ee = e * p.e;
	float mm = mu + p.mu;

	Vector3f vvn = -ee * vn;
	Vector3f vvt;
	float v = vt.norm2() - (1 + ee)*mm*vn.norm2();
	if (v > 0)
		vvt = v * vt.normalize();
	else
		vvt = Vector3f(0, 0, 0);

	velo = vvn + vvt;
}

void Sphere::collide(vector<Sphere>& spheres,
		const vector<Plane>& planes)
{
	for (vector<Sphere>::iterator its = spheres.begin();
			its != spheres.end(); its++)
		checkAndChange(*its);
	for (vector<Plane>::const_iterator itp = planes.begin();
			itp != planes.end(); itp++)
		checkAndChange(*itp);
}
