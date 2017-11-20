#ifndef H_PLANE
#define H_PLANE

#include <glm/glm.hpp>
#include "Scene.hpp"
using namespace glm;
class Plane : public Scene
{
  public:
    vec3 a, b, c, d;
    Plane(void);

    Plane(vec3 pa, vec3 pb, vec3 pc, vec3 pd, vec3 col) : a(pa), b(pb), c(pc), d(pd)
    {
      color = col;
    };

    bool intersected(vec3 pt);

    float intersection(vec3 posn, vec3 dir);

    vec3 normal(vec3 pt);

};

#endif
