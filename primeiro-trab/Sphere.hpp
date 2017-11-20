#ifndef H_SPHERE
#define H_SPHERE
#include <glm/glm.hpp>
#include "Scene.hpp"

using namespace glm;
class Sphere:public Scene
{
  public:
  vec3 center;
  float radius;
  Sphere() : center(vec3(0.f)), radius(1)
  {
    color = vec3(1);
  };

  Sphere(vec3 c, float r, vec3 col) : center(c), radius(r)
  {
    color = col;
  };

  float intersection(vec3 posn, vec3 dir);

  vec3 normal(vec3 p);

};

#endif
