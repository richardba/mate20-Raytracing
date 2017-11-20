#include "Sphere.hpp"
#include <math.h>

using namespace glm;
/**
* Sphere's intersection method.  The input is a ray (pos, dir).
*/
float Sphere::intersection(vec3 posn, vec3 dir)
{
    vec3 vdif = posn - center;
    float b = dot(dir, vdif);
    float len = length(vdif);
    float c = len*len - radius*radius;
    float delta = b*b - c;

	if(fabs(delta) < 0.001f) return -1.0f;
    if(delta < 0.0f) return -1.0f;

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    if(fabs(t1) < 0.001f )
    {
        if (t2 > 0.f) return t2;
        else t1 = -1.0f;
    }
    if(fabs(t2) < 0.001f ) t2 = -1.0f;

	return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
vec3 Sphere::normal(vec3 p)
{
    vec3 n = p - center;
    n = normalize(n);
    return n;
}
