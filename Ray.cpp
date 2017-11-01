#include "Ray.hpp"

using namespace glm;

void Ray::normalize()
{
  direction = glm::normalize(direction);
}

void Ray::closestPt(std::vector<Scene*> &Scenes)
{
  vec3 point(0.f,0.f,0.f);
  float min = 1.e+6;
  for(int i = 0;  i < (int) Scenes.size();  i++)
  {
    float t = Scenes[i]->intersection(pt, direction);
    if(t > 0.f)
    {
      point = pt + direction*t;
      if(t < min)
      {
        xpt = point;
        xindex = i;
        xdist = t;
        min = t;
      }
    }
  }
}

