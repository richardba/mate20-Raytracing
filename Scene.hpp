#ifndef H_SOBJECT
#define H_SOBJECT
#include <glm/glm.hpp>

using namespace glm;
class Scene
{
  public:
    vec3 color;
    Scene() {}
    virtual float intersection(vec3 pos, vec3 dir) = 0;
    virtual vec3 normal(vec3 pos) = 0;
    virtual ~Scene() {}
    vec3 getColor();
    void setColor(vec3 col);
};

#endif
