#include "Plane.hpp"
#include <math.h>

using namespace glm;

bool Plane::intersected(vec3 pt)
{
	vec3 n = normal(pt);

	vec3 ua = b - a;
	vec3 ub = c - b;
	vec3 uc = d - c;
	vec3 ud = a - d;

	vec3 va = pt - a;
	vec3 vb = pt - b;
	vec3 vc = pt - c;
	vec3 vd = pt - d;

	vec3 cross_a = cross(ua, va);
	vec3 cross_b = cross(ub, vb);
	vec3 cross_c = cross(uc, vc);
	vec3 cross_d = cross(ud, vd);

	float dot_a = dot(cross_a, n);
	float dot_b = dot(cross_b, n);
	float dot_c = dot(cross_c, n);
	float dot_d = dot(cross_d, n);

	if (dot_a > 0.f && dot_b > 0.f && dot_c > 0.f && dot_d > 0.f)
  {
		return true;
	}
	else
	{
		return false;
	}
}

float Plane::intersection(vec3 posn, vec3 dir)
{
	vec3 n = normal(posn);
	vec3 vdif = a - posn;
	float vdotn = dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	vec3 q = posn + dir*t;
	if(intersected(q)) return t;
    else return -1;
}

vec3 Plane::normal(vec3 pt)
{

	vec3 w = cross((b-a), (d-a));
	vec3 norm = normalize(w);

    return norm;
}



