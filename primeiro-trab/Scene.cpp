#include "Scene.hpp"

using namespace glm;
vec3 Scene::getColor()
{
	return color;
}

void Scene::setColor(vec3 col)
{
	color = col;
}
