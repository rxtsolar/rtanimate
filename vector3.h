#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <cmath>
#include <iostream>
using namespace std;

template <class T> class Vector3;
typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

template <class T>
class Vector3 {
private:
    T v[3];

public:
    Vector3()
    {
        v[0] = v[1] = v[2] = 0;
    }

    Vector3(const T& value)
    {
        v[0] = v[1] = v[2] = value;
    }

    Vector3(const T& a, const T& b, const T& c)
    {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }

    Vector3(const Vector3<T>& vect)
    {
        v[0] = vect.v[0];
        v[1] = vect.v[1];
        v[2] = vect.v[2];
    }

    const T& x() const
    {
        return v[0];
    }

    const T& y() const
    {
        return v[1];
    }

    const T& z() const
    {
        return v[2];
    }

    T& x()
    {
        return v[0];
    }

    T& y()
    {
        return v[1];
    }

    T& z()
    {
        return v[2];
    }

    const T& operator[](int i) const
    {
        return v[i];
    }

    T& operator[](int i)
    {
        return v[i];
    }

    Vector3<T> operator+(const Vector3<T>& vect) const
    {
        return Vector3<T>(v[0] + vect[0], v[1] + vect[1], v[2] + vect[2]);
    }

    Vector3<T> operator-(const Vector3<T>& vect) const
    {
        return Vector3<T>(v[0] - vect[0], v[1] - vect[1], v[2] - vect[2]);
    }

    Vector3<T> operator*(const T& fact) const
    {
        return Vector3<T>(v[0]*fact, v[1]*fact, v[2]*fact);
    }

    Vector3<T> operator*(const Vector3<T>& vect) const
    {
        return Vector3<T>(v[0] * vect[0], v[1] * vect[1], v[2] * vect[2]);
    }

    Vector3<T> operator/(const Vector3<T>& vect) const
    {
        return Vector3<T>(v[0] / vect[0], v[1] / vect[1], v[2] / vect[2]);
    }

    Vector3<T> operator/(const T& fact) const
    {
        return Vector3<T>(v[0]/fact, v[1]/fact, v[2]/fact);
    }

    friend Vector3<T> operator*(const T& fact, const Vector3<T>& vect)
    {
        return Vector3<T>(fact*vect[0], fact*vect[1], fact*vect[2]);
    }

    Vector3<T> operator-() const
    {
        return Vector3<T>(-v[0], -v[1], -v[2]);
    }

    Vector3<T>& operator=(const Vector3<T>& vect)
    {
        v[0] = vect[0];
        v[1] = vect[1];
        v[2] = vect[2];
        return *this;
    }

    Vector3<T>& operator+=(const Vector3<T>& vect)
    {
        return (*this) = (*this) + vect;
    }

    Vector3<T>& operator-=(const Vector3<T>& vect)
    {
        return (*this) = (*this) - vect;
    }

    Vector3<T>& operator*=(const T& fact)
    {
        return (*this) = (*this)*fact;
    }

    Vector3<T>& operator/=(const T& fact)
    {
        return (*this) = (*this)/fact;
    }

    bool operator!=(const Vector3<T>& vect)
    {
        return v[0] != vect[0] || v[1] != vect[1] || v[2] != vect[2];
    }

    T dot(const Vector3<T>& vect) const
    {
        return v[0]*vect[0] + v[1]*vect[1] + v[2]*vect[2];
    }

    Vector3<T> cross(const Vector3<T>& vect) const
    {
        return Vector3<T>(v[1]*vect[2] - v[2]*vect[1],
                v[2]*vect[0] - v[0]*vect[2],
                v[0]*vect[1] - v[1]*vect[0]);
    }

    T norm1() const
    {
        return fabs(v[0]) + fabs(v[1]) +fabs(v[2]);
    }

    T norm2() const
    {
        return sqrt(dot(*this));
    }

    T dist(const Vector3<T>& vect) const
    {
        return (*this - vect).norm2();
    }

    Vector3<T> normalize() const
    {
        return (*this)/(this->norm2());
    }

    friend Vector3<T> max(const Vector3<T>& v0, const Vector3<T>& v1)
    {
        T x = v0[0] > v1[0] ? v0[0] : v1[0];
        T y = v0[1] > v1[1] ? v0[1] : v1[1];
        T z = v0[2] > v1[2] ? v0[2] : v1[2];
        return Vector3<T>(x, y, z);
    }

    friend Vector3<T> min(const Vector3<T>& v0, const Vector3<T>& v1)
    {
        T x = v0[0] < v1[0] ? v0[0] : v1[0];
        T y = v0[1] < v1[1] ? v0[1] : v1[1];
        T z = v0[2] < v1[2] ? v0[2] : v1[2];
        return Vector3<T>(x, y, z);
    }
};

#endif
