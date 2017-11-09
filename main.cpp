#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include <math.h>
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
const int recursionLevel = 5;
const int resolutionLevel = 200;

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

void keyboardSpecial(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_LEFT :
      theta += 2.f;
      if(theta>360.f)
        theta=-360.f;
      glutPostRedisplay();
      break;
    case GLUT_KEY_RIGHT :
      theta -= 2.f;
      if(theta<-360.f)
        theta=360.f;
      glutPostRedisplay();
      break;
    case GLUT_KEY_UP :
      phi -= 2.f;
      if(phi<-360.f)
        phi=360.f;
      glutPostRedisplay();
      break;
		case GLUT_KEY_DOWN :
		  phi += 2.f;
      if(phi<-360.f)
        phi=360.f;
      glutPostRedisplay();
      break;
  }
}
void keyboard(unsigned char key, int x, int y)
{

  switch (key)
  {
    case 'B':
    case 'b':
      ballMode =! ballMode;
      lightMode = false;
      cout << "Ballmode: " << ballMode << endl;
      break;
    case 'L':
    case 'l':
      lightMode =! lightMode;
      ballMode = false;
      cout << "Lightmode: " << lightMode << endl;
      break;
    case 27: // ESCAPE key
      exit (0);
  }

  if(!ballMode&&!lightMode)
    switch (key)
    {
      case 'W':
      case 'w':
        atZ -= 2.f;
        glutPostRedisplay();
        break;
      case 'S':
      case 's':
        atZ += 2.f;
        glutPostRedisplay();
        break;
      case 'A':
      case 'a':
        atX -= 2.f;
        glutPostRedisplay();
        break;
      case 'D':
      case 'd':
        atX += 2.f;
        glutPostRedisplay();
        break;
      case 'c':
      case 'C':
        atY -= 2.f;
        glutPostRedisplay();
        break;
      case 32: // ESPACE BAR key
        atY += 2.f;
        glutPostRedisplay();
        break;
      case 'm':
        focalDistance += 1;
        glutPostRedisplay();
        break;
      case 'n':
        focalDistance -= 1;
        glutPostRedisplay();
        break;
    }
    else if(lightMode)
      switch(key)
      {
        case 'W':
        case 'w':
          light.z = light.z - 2.f;
          glutPostRedisplay();
          break;
        case 'S':
        case 's':
          light.z = light.z + 2.f;
          glutPostRedisplay();
          break;
        case 'A':
        case 'a':
          light.x = light.x - 2.f;
          glutPostRedisplay();
          break;
        case 'D':
        case 'd':
          light.x = light.x + 2.f;
          glutPostRedisplay();
          break;
        case 'c':
        case 'C':
          light.y = light.y - 2.f;
          glutPostRedisplay();
          break;
        case 32: // ESPACE BAR key
          light.y = light.y + 2.f;
          glutPostRedisplay();
          break;
      }
    else
      switch(key)
      {
        case 'W':
        case 'w':
          sphere1->center.z = sphere1->center.z - 2.f;
          glutPostRedisplay();
          break;
        case 'S':
        case 's':
          sphere1->center.z = sphere1->center.z + 2.f;
          glutPostRedisplay();
          break;
        case 'A':
        case 'a':
          sphere1->center.x = sphere1->center.x - 2.f;
          glutPostRedisplay();
          break;
        case 'D':
        case 'd':
          sphere1->center.x = sphere1->center.x + 2.f;
          glutPostRedisplay();
          break;
        case 'c':
        case 'C':
          sphere1->center.y = sphere1->center.y - 2.f;
          glutPostRedisplay();
          break;
        case 32: // ESPACE BAR key
          sphere1->center.y = sphere1->center.y + 2.f;
          glutPostRedisplay();
          break;
      }
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
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBegin(GL_QUADS);

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

      vec3 col;
      col.r = (col1.r + col2.r + col3.r + col4.r) / 4;
      col.g = (col1.g + col2.g + col3.g + col4.g) / 4;
      col.b = (col1.b + col2.b + col3.b + col4.b) / 4;

      glColor3f(col.r, col.g, col.b);
      glVertex2f(xp, yp);
      glVertex2f(xp+cellX, yp);
      glVertex2f(xp+cellX, yp+cellY);
      glVertex2f(xp, yp+cellY);
    }
  }

  glEnd();
  glFlush();
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
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(bandWidth, bandHeight, minimal, maximal);
  glClearColor(0, 0, 0, 1);

	createSpheres();
	createFloor();
	createBox();
	char* s = (char *)"Texture.bmp";
	texture = TextureBMP(s);
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
  glutInitWindowSize(300.f, 300.f);
  glutInitWindowPosition(20.f, 20.f);
  glutCreateWindow("Ray Tracer");
  initialize();
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecial);
  glutMainLoop();
  return 0;
}
