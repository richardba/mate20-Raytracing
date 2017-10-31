/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c, d.
*/
bool Plane::isInside(glm::vec3 pt)
{
	glm::vec3 n = normal(pt);
	
	glm::vec3 ua = b - a;
	glm::vec3 ub = c - b;
	glm::vec3 uc = d - c;
	glm::vec3 ud = a - d;
	
	glm::vec3 va = pt - a;
	glm::vec3 vb = pt - b;
	glm::vec3 vc = pt - c;
	glm::vec3 vd = pt - d;
	
	glm::vec3 cross_a = glm::cross(ua, va);
	glm::vec3 cross_b = glm::cross(ub, vb);
	glm::vec3 cross_c = glm::cross(uc, vc);
	glm::vec3 cross_d = glm::cross(ud, vd);
	
	float dot_a = glm::dot(cross_a, n);
	float dot_b = glm::dot(cross_b, n);
	float dot_c = glm::dot(cross_c, n);
	float dot_d = glm::dot(cross_d, n);
	
	if (dot_a > 0 && dot_b > 0 && dot_c > 0 && dot_d > 0) {
		return true;
	} else {
		return false;
	}
}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
float Plane::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 n = normal(posn);
	glm::vec3 vdif = a - posn;
	float vdotn = glm::dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	glm::vec3 q = posn + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the plane's normal vector using 
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Plane::normal(glm::vec3 pt)
{

	glm::vec3 w = cross((b-a), (d-a));
	glm::vec3 norm = glm::normalize(w);

    return norm;
}



