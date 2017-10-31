/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinders's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
	float b = (dir.x*(posn.x - center.x) + dir.z*(posn.z - center.z));
	float c = ((posn.x - center.x)*(posn.x - center.x) + (posn.z - center.z)*(posn.z - center.z) - radius*radius);
	float delta = b*b - c;
	
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;
    
    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;

	return (t1 < t2)? t1: t2;
    
}
//~ {
    //~ glm::vec3 vdif = posn - center;
    //~ float b = glm::dot(dir, vdif);
    //~ float len = glm::length(vdif);
    //~ float c = len*len - radius*radius;
    //~ float delta = b*b - c;
   
	//~ if(fabs(delta) < 0.001) return -1.0; 
    //~ if(delta < 0.0) return -1.0;

    //~ float t1 = -b - sqrt(delta);
    //~ float t2 = -b + sqrt(delta);
    //~ if(fabs(t1) < 0.001 )
    //~ {
        //~ if (t2 > 0) return t2;
        //~ else t1 = -1.0;
    //~ }
    //~ if(fabs(t2) < 0.001 ) t2 = -1.0;

	//~ return (t1 < t2)? t1: t2;
//~ }

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n(p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
    return n;
}
