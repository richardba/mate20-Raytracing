#ifndef H_RAY
#define H_RAY
#include <glm/glm.hpp>
#include <vector>
#include "Scene.hpp"
using namespace glm;
class Ray
{

  public:
    vec3 pt;
    vec3 direction;
    vec3 xpt;
    int xindex;
    float xdist;

    Ray()
    {
      pt = vec3(0.f, 0.f, 0.f);
      direction = vec3(0.f, 0.f, -1);
      xpt = vec3(0.f, 0.f, 0.f);
      xindex = -1;
      xdist = 0;
    };

    Ray(vec3 point, vec3 dir)	: pt(point), direction(dir)
    {
      xpt = vec3(0.f, 0.f, 0.f);
      xindex = -1;
      xdist = 0;
    };

    void normalize();
    void closestPt(std::vector<Scene*> &Scenes);

};
#endif
