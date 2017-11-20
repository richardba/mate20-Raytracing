#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "TextureBMP.h"

using namespace glm;
float drand()
{
  srand(time(0)+rand());
  return (double)rand()/RAND_MAX;
}

using namespace std;

//Controle do nível de resolução e recursão
const int recursionLevel = 3;
const int resolutionLevel = 200;
const int resolutionLevel2 = resolutionLevel*resolutionLevel;

const float bandWidth = -25.f;
const float bandHeight =  25.f;
const float minimal = -25.f;
const float maximal =  25.f;

//Váriaveis para mudança de posição
float atX=0.f, atY = 0.f, atZ = 0.f, theta=1.f, phi=1.f, focalDistance = 40.0f;
Sphere *sphere1;
bool ballMode=false,
     lightMode=false;

vec3 light(9.f, 35.f, -4.f);

TextureBMP texture;
vector<Scene*> Scenes;

vec3 image[resolutionLevel2];

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
  if(key== GLFW_KEY_B && (action == GLFW_PRESS||action == GLFW_REPEAT))
  {
    ballMode =! ballMode;
    lightMode = false;
  }
  if(key== GLFW_KEY_L && (action == GLFW_PRESS||action == GLFW_REPEAT))
  {
    lightMode =! lightMode;
    ballMode = false;
  }
  if(!ballMode&&!lightMode)
  {
    if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS||action == GLFW_REPEAT))
      theta += 2.f;
    if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS||action == GLFW_REPEAT))
      theta -= 2.f;
    if(key == GLFW_KEY_UP && (action == GLFW_PRESS||action == GLFW_REPEAT))
      phi -= 2.f;
    if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS||action == GLFW_REPEAT))
      phi += 2.f;
    if(key == GLFW_KEY_W && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atZ -= 2.f;
    if(key == GLFW_KEY_S && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atZ += 2.f;
    if(key == GLFW_KEY_A && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atX -= 2.f;
    if(key == GLFW_KEY_D && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atX += 2.f;
    if(key == GLFW_KEY_C && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atY -= 2.f;
    if(key == GLFW_KEY_SPACE && (action == GLFW_PRESS||action == GLFW_REPEAT))
      atY += 2.f;
    if(key == GLFW_KEY_M && (action == GLFW_PRESS||action == GLFW_REPEAT))
      focalDistance += 1;
    if(key == GLFW_KEY_N && (action == GLFW_PRESS||action == GLFW_REPEAT))
      focalDistance -= 1;
  }
  else if(lightMode)
  {
    if(key == GLFW_KEY_W && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.z = light.z - 2.f;
    if(key == GLFW_KEY_S && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.z = light.z + 2.f;
    if(key == GLFW_KEY_A && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.x = light.x - 2.f;
    if(key == GLFW_KEY_D && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.x = light.x + 2.f;
    if(key == GLFW_KEY_C && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.y = light.y - 2.f;
    if(key == GLFW_KEY_SPACE && (action == GLFW_PRESS||action == GLFW_REPEAT))
      light.y = light.y + 2.f;
  }
  else
  {
    if(key == GLFW_KEY_W && (action == GLFW_PRESS||action == GLFW_REPEAT))
      sphere1->center.z = sphere1->center.z - 2.f;
    if(key == GLFW_KEY_S && (action == GLFW_PRESS||action == GLFW_REPEAT))
      sphere1->center.z = sphere1->center.z + 2.f;
    if(key == GLFW_KEY_A && (action == GLFW_PRESS||action == GLFW_REPEAT))
      sphere1->center.x = sphere1->center.x - 2.f;
    if(key == GLFW_KEY_D && (action == GLFW_PRESS||action == GLFW_REPEAT))
      sphere1->center.x = sphere1->center.x + 2.f;
    if(key == GLFW_KEY_C && (action == GLFW_PRESS||action == GLFW_REPEAT))
      sphere1->center.y = sphere1->center.y - 2.f;
    if(key == GLFW_KEY_SPACE && (action == GLFW_PRESS||action == GLFW_REPEAT)) // ESPACE BAR key
      sphere1->center.y = sphere1->center.y + 2.f;
  }
  if(theta<-360.f)
    theta=360.f;
  if(theta>360.f)
    theta=-360.f;
}

vec3 proceduralTexture(Ray ray, float x, float y, float r, vec3 col1, vec3 col2, vec3 col3)
{
	vec3 col;
	if(ray.xpt.x < (x-r+(r/4.f)) || (ray.xpt.x >= (x-r+(3.f*r/4.f)) && ray.xpt.x < (x+r-(3.f*r/4.f))) || ray.xpt.x >= (x+r-(r/4.f)))
  {
		if(ray.xpt.y < (y-r+(r/4.f)) || (ray.xpt.y >= (y-r+(3.f*r/4.f)) && ray.xpt.y < (y+r-(3.f*r/4.f))) || ray.xpt.y >= (y+r-(r/4.f)))
    {
			return col1;
		}
		else
		{
			return col2;
		}
	}
	else
  {
		if(ray.xpt.y < (y-r+(r/4.f)) || (ray.xpt.y >= (y-r+(3.f*r/4.f)) && ray.xpt.y < (y+r-(3.f*r/4.f))) || ray.xpt.y >= (y+r-(r/4.f)))
		{
			return col3;
		} else
		{
			return col1;
		}
	}
}


vec3 rayTracing(Ray ray, int step)
{
	vec3 resultingColor;
	float ambientLight = 0.15f;
	vec3 clearColor(.1f,.3f,.7f);

  ray.intersect(Scenes);
  if(ray.xindex == -1) return clearColor;
  vec3 sceneColor = Scenes[ray.xindex]->getColor();

	vec3 normalVector = Scenes[ray.xindex]->normal(ray.xpt);
	vec3 lightPosition = light - ray.xpt;
	vec3 lightNormal = normalize(lightPosition);
	float dotProduct = dot(lightNormal, normalVector);

	vec3 reflectVector = reflect(-lightPosition, normalVector);
	vec3 reflectNormal = normalize(reflectVector);
	float spec = dot(reflectNormal, -ray.direction);

	if(spec < 0.f)
  {
		spec = 0.f;
	}
	else if (ray.xindex == 1)
  {
		spec = pow(spec, 39.99f);
	}
	else
  {
		spec = pow(spec, 6.99f);
	}

	Ray shadow(ray.xpt, lightNormal);
	shadow.intersect(Scenes);

	float rayDistance = glm::distance(ray.xpt, light);

	if(ray.xindex == 0 && step < recursionLevel)
  {
		vec3 reflectedDir = reflect(ray.direction, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		vec3 reflectedPosition = rayTracing(reflectedRay, step+1);
		resultingColor = resultingColor + (0.8f*reflectedPosition);
	}
	else if(ray.xindex == 1 && step < recursionLevel)
  {
		float eta = 1.f/1.005f;
		vec3 g = refract(ray.direction, normalVector, eta);
		Ray refractRay1(ray.xpt, g);
		refractRay1.intersect(Scenes);
		vec3 m = Scenes[refractRay1.xindex]->normal(refractRay1.xpt);
		vec3 h = refract(g, -m, 1.0f/eta);
		Ray refractRay2(refractRay1.xpt, h);
		vec3 refractedCol = rayTracing(refractRay2, step);
		resultingColor = resultingColor + (0.8f*refractedCol);
	}
	else if (ray.xindex == 2)
  {
		vec3 col1(1.f, 0.f, 0.f);
		vec3 col2(0.f, 1, 0.f);
		vec3 col3(0.f, 0.f, 1);

		sceneColor = proceduralTexture(ray, 8.f, 0.f, 4.f, col1, col2, col3);
	}
	else if(ray.xindex == 4)
	{
		float texcoordx = (ray.xpt.x + 200.f)/(200.f - -200.f);
		float texcoordz = (ray.xpt.z + 0.f)/(-200.f);
		sceneColor = texture.getColorAt(texcoordx, texcoordz);
	}

	if((dotProduct <= 0.f) || ((shadow.xindex > -1) && (shadow.xdist < rayDistance)))
	{
		if(shadow.xindex == 1)
    {
			return (sceneColor * 0.67f + resultingColor);
		}
		else
    {
			return (sceneColor * ambientLight + resultingColor);
		}
	}
	else
  {
		return (ambientLight * sceneColor + dotProduct * sceneColor + spec + resultingColor);
	}
}

void display()
{
  float cellX = (bandHeight-bandWidth)/resolutionLevel,
        cellY = (maximal-minimal)/resolutionLevel,
        xp, yp;

  vec3 eye(atX, atY, atZ);

  glClear(GL_COLOR_BUFFER_BIT);


  for(int i = 0; i < resolutionLevel; i++)
  {
    xp = bandWidth + i*cellX;
    for(int j = 0; j < resolutionLevel; j++)
    {
      yp = minimal + j*cellY;


      vec3 uleft((xp+0.25*cellX)-theta, yp+0.75*cellY+phi, -focalDistance+M_PI*(phi*theta/focalDistance)/180);
      vec3 uright((xp+0.75*cellX)-theta, yp+0.75*cellY+phi, -focalDistance+M_PI*(phi*theta/focalDistance)/180);
      vec3 lleft((xp+0.25*cellX)-theta, yp+0.25*cellY+phi, -focalDistance+M_PI*(phi*theta/focalDistance)/180);
      vec3 lright((xp+0.75*cellX)-theta, yp+0.25*cellY+phi, -focalDistance+M_PI*(phi*theta/focalDistance)/180);

      Ray ray1 = Ray(eye, uleft);
      ray1.normalize();
      Ray ray2 = Ray(eye, uright);
      ray2.normalize();
      Ray ray3 = Ray(eye, lleft);
      ray3.normalize();
      Ray ray4 = Ray(eye, lright);
      ray4.normalize();

      vec3 col1 = rayTracing(ray1, 1);
      vec3 col2 = rayTracing(ray2, 1);
      vec3 col3 = rayTracing(ray3, 1);
      vec3 col4 = rayTracing(ray4, 1);


      //% 256 ) * 256 * 256 * 256
      vec3 col;
      col.r = (col1.r + col2.r + col3.r + col4.r) / 4;
      col.g = (col1.g + col2.g + col3.g + col4.g) / 4;
      col.b = (col1.b + col2.b + col3.b + col4.b) / 4;
      image[resolutionLevel*j+i] = col;
    }
  }
}

void createSpheres()
{
	sphere1 = new Sphere(vec3(-5.0f, 11.0f, -110.0f), 20.0f, vec3(0.1f, 0.2f, drand()));
	Sphere *sphere2 = new Sphere(vec3(-3.f, -11.f, -70.0f), 5.0f, vec3(1.f, drand(), 0.f));
	Sphere *sphere3 = new Sphere(vec3(8.0f, 0.0f, -85.0f), 4.0f, vec3(1.f, 0.f, 0.f));
	Sphere *sphere4 = new Sphere(vec3(-12.5f, -12.5f, -82.5), 4.0f, vec3(drand(), drand(), drand()));
	Scenes.push_back(sphere1);
	Scenes.push_back(sphere2);
	Scenes.push_back(sphere3);
	Scenes.push_back(sphere4);
}



void createFloor()
{

	Plane *plane = new Plane (vec3(-200.f, -20.f, 0.f),
                            vec3(200.f, -20.f, 0.f),
                            vec3(200.f, -20.f, -200.f),
                            vec3(-200.f, -20.f, -200.f),
                            vec3(1));
	Scenes.push_back(plane);
}


void createBox()
{
	vec3 boxColor1 = vec3(drand(),drand(),drand());
	vec3 boxColor2 = vec3(boxColor1.x/2,boxColor1.y/2,boxColor1.z/2);
	Plane *bottom = new Plane (vec3(6.f, -15.f, -80.f),
								vec3(4.f, -15.f, -84.f),
								vec3(9.f, -15.f, -84.f),
								vec3(11.f, -15.f, -80.f),
								boxColor1);

	Plane *top = new Plane (vec3(6.f, -10.f, -80.f),
								vec3(11.f, -10.f, -80.f),
								vec3(9.f, -10.f, -84.f),
								vec3(4.f, -10.f, -84.f),
								boxColor2);

	Plane *front = new Plane (vec3(6.f, -10.f, -80.f),
								vec3(6.f, -15.f, -80.f),
								vec3(11.f, -15, -80.f),
								vec3(11.f, -10.f, -80.f),
								boxColor2);

	Plane *right = new Plane (vec3(11, -10.f, -80.f),
								vec3(11, -15, -80.f),
								vec3(9, -15, -84),
								vec3(9, -10.f, -84),
								boxColor2);

	Plane *back = new Plane (vec3(9, -10.f, -84),
								vec3(9, -15, -84),
								vec3(4, -15, -84),
								vec3(4, -10.f, -84),
								boxColor2);

	Plane *left = new Plane (vec3(4, -10.f, -84),
								vec3(4, -15, -84),
								vec3(6, -15, -80.f),
								vec3(6, -10.f, -80.f),
								boxColor2);

	Scenes.push_back(bottom);
	Scenes.push_back(top);
	Scenes.push_back(front);
	Scenes.push_back(right);
	Scenes.push_back(back);
	Scenes.push_back(left);
}


void initialize()
{
	createSpheres();
	createFloor();
	createBox();
	char* s = (char *)"Texture.bmp";
	texture = TextureBMP(s);
}

void drawRayTracing()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  display();
  glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, sizeof(glm::vec3)*resolutionLevel2, image);
  glDrawPixels(resolutionLevel, resolutionLevel, GL_RGB, GL_FLOAT, 0);
}

int main(int argc, char* argv[])
{
  GLFWwindow* window;
  	// Initialise GLFW
	if( !glfwInit() )
	{
		cout << "Erro ao inicializar o GLFW!" << endl;
		cin >> argc;
    return EXIT_FAILURE;
	}

  glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( resolutionLevel, resolutionLevel, "Ray Tracer", NULL, NULL);
	if( window == NULL )
  {
    cout << "Erro ao inicializar o GLFW!" << endl;
		cin >> argc;
    return EXIT_FAILURE;
	}

	glfwSetKeyCallback(window, keyCallback);
	const GLFWvidmode mode = *glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwSetWindowPos(window, (mode.width-resolutionLevel)/2, (mode.height-resolutionLevel)/2);
  glfwShowWindow(window);
  glfwMakeContextCurrent(window);
  glClearColor(0.1f, 0.1f, drand(), 1.f);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glewExperimental=GL_TRUE;
	if (glewInit() != GLEW_OK)
  {
		cout << "Erro ao inicializar o GLEW!" << endl;
		cin >> argc;
		glfwTerminate();
		return EXIT_FAILURE;
  }

  unsigned int PBO;
  glGenBuffers(1, &PBO);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(glm::vec3)*resolutionLevel2, image, GL_DYNAMIC_DRAW);

 	initialize();

	do
	{
	  drawRayTracing();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
  glfwTerminate();

  return EXIT_SUCCESS;
}
