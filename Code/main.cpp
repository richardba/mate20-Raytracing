/*========================================================================
 * COSC363 Assignment 2 - Ray Tracing Assignment
 * Ben Ridgen - 14852010
 * This program creates a scene consisting of geometrical objects with
 * different properties.
*=========================================================================
*/


//include statements
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <math.h>
#include "Sphere.h"
#include "Plane.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include <GL/glut.h>

using namespace std;

//global constants
const float WIDTH = 50.0;
const float HEIGHT = 50.0;
const float EDIST = 40.0;
const int NUMDIV = 250;
const int MAX_STEPS = 6;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
float atX=0.f, atY = 0.f, atZ = 0.f;

//declaring the texture
TextureBMP texture;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:             // ESCAPE key
      exit (0);
      break;
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
      cout << cos(atX*M_PI/180)+sin(atX*M_PI/180) << endl;
      glutPostRedisplay();
      break;
    case 'D':
    case 'd':
      atX += 2.f;
      cout << cos(atX*M_PI/180)+sin(atX*M_PI/180) << endl;
      glutPostRedisplay();
      break;
  }
}

//----- The procedural texturing function -------------------
//    Used for procedural generation of a texture on a sphere
//      x, y, and r are parameters of the sphere itself
//       col1, col2 and col3 are the colours to be used
//-------------------------------------------------------------
glm::vec3 proceduralTexture(Ray ray, float x, float y, float r, glm::vec3 col1, glm::vec3 col2, glm::vec3 col3)
{
	glm::vec3 col;
	if(ray.xpt.x < (x-r+(r/4)) || (ray.xpt.x >= (x-r+(3*r/4)) && ray.xpt.x < (x+r-(3*r/4))) || ray.xpt.x >= (x+r-(r/4))){
		if(ray.xpt.y < (y-r+(r/4)) || (ray.xpt.y >= (y-r+(3*r/4)) && ray.xpt.y < (y+r-(3*r/4))) || ray.xpt.y >= (y+r-(r/4))){
			return col1;
		} else {
			return col2;
		}
	} else {
		if(ray.xpt.y < (y-r+(r/4)) || (ray.xpt.y >= (y-r+(3*r/4)) && ray.xpt.y < (y+r-(3*r/4))) || ray.xpt.y >= (y+r-(r/4))){
			return col3;
		} else {
			return col1;
		}
	}
}


//---The ray tracing function ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(.1f,.3f,.7f);   //Colour fo the background
	glm::vec3 light(10, 40, -3);  //Lights Position
	float ambientTerm = 0.2;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 col = sceneObjects[ray.xindex]->getColor(); //else return object's colour

	glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);   //calculating normal vector of intersection
	glm::vec3 lightVector = light - ray.xpt;   //calculating the light vector
	glm::vec3 lightNormal = glm::normalize(lightVector);   //normalizing
	float lDotn = glm::dot(lightNormal, normalVector);       //Gets the normal vector for light

	glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
	glm::vec3 reflNormal = glm::normalize(reflVector);    //calculated reflected vector then normalized
	float spec = glm::dot(reflNormal, -ray.dir);         //specular reflection

	//calculates the specular component of the position that the ray has intersected
	if(spec < 0){
		spec = 0;
	} else if (ray.xindex == 1) {
		spec = glm::pow(spec, 40.0);
	} else {
		spec = glm::pow(spec, 7.0);
	}

	Ray shadow(ray.xpt, lightNormal);
	shadow.closestPt(sceneObjects);    //creation of a shadow ray and closest intersect

	float d = glm::distance(ray.xpt, light);     //distance between object and light source

	glm::vec3 colorSum;   //initiating colorSum

	if(ray.xindex == 0 && step < MAX_STEPS){   //creating the reflective sphere
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
		colorSum = colorSum + (0.8f*reflectedCol);
	} else if(ray.xindex == 1 && step < MAX_STEPS){   //creating the refractive, transparent sphere
		float eta = 1/1.005;
		glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
		Ray refrRay1(ray.xpt, g);
		refrRay1.closestPt(sceneObjects);
		glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
		glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
		Ray refrRay2(refrRay1.xpt, h);
		glm::vec3 refractedCol = trace(refrRay2, step); //Recursion!
		colorSum = colorSum + (0.8f*refractedCol);
	} else if (ray.xindex == 2){    //procedural generation of colours on 'red' sphere
		glm::vec3 col1(1, 0, 0);
		glm::vec3 col2(0, 1, 0);
		glm::vec3 col3(0, 0, 1);
		//col1 appears the most in the texture while col2 and col3 appear the least
		col = proceduralTexture(ray, 8, 0, 4, col1, col2, col3);  //x, y and r from sphere
	} else if(ray.xindex == 4){    //Texturing the floor plane
		float texcoordx = (ray.xpt.x + 200)/(200 - -200);
		float texcoordz = (ray.xpt.z + 0)/(-200);
		col = texture.getColorAt(texcoordx, texcoordz);
	}

	//determining if the object is in shadow or not and returning a colour value accordingly
	if((lDotn <= 0) || ((shadow.xindex > -1) && (shadow.xdist < d))) {
		if(shadow.xindex == 1) {
			return (col * 0.67f + colorSum);
		} else {
			return (col * ambientTerm + colorSum);
		}
	} else {
		return (ambientTerm * col + lDotn * col + spec + colorSum);
	}
}


//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

  float trackball = M_PI*std::sqrt(abs(atX))/180;
	glm::vec3 eye(trackball+atX/2, atY, atZ);  //The eye position (source of primary rays) is the origin
	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

			//divided each square pixel into four to perform supersampling
		    glm::vec3 uleft(xp+0.25*cellX*trackball, yp+0.75*cellY, -EDIST);	//direction of the primary ray
		    glm::vec3 uright(xp+0.75*cellX*trackball, yp+0.75*cellY, -EDIST);
		    glm::vec3 lleft(xp+0.25*cellX*trackball, yp+0.25*cellY, -EDIST);
		    glm::vec3 lright(xp+0.75*cellX*trackball, yp+0.25*cellY, -EDIST);

		    Ray ray1 = Ray(eye, uleft);		//Create a ray originating from the camera in the direction 'dir'
			ray1.normalize();				//Normalize the direction of the ray to a unit vector
			Ray ray2 = Ray(eye, uright);
			ray2.normalize();
			Ray ray3 = Ray(eye, lleft);
			ray3.normalize();
			Ray ray4 = Ray(eye, lright);
			ray4.normalize();

		    glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
		    glm::vec3 col2 = trace (ray2, 1);
		    glm::vec3 col3 = trace (ray3, 1);
		    glm::vec3 col4 = trace (ray4, 1);

		    glm::vec3 col;    //finding tthe average colour of the four rays in the pixel
		    col.r = (col1.r + col2.r + col3.r + col4.r) / 4;
		    col.g = (col1.g + col2.g + col3.g + col4.g) / 4;
		    col.b = (col1.b + col2.b + col3.b + col4.b) / 4;

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//------ This sphere creation function ------------
//    Creates instances of the sphere class to
//		be displayed in the scene
//--------------------------------------------------
void createSpheres()
{
	//-- Creating pointers to sphere objects
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -1.0, -110.0), 20.0, glm::vec3(0.1, 0.2, 0.75));   //Blue Reflecting sphere
	Sphere *sphere2 = new Sphere(glm::vec3(-3, -11, -70.0), 5.0, glm::vec3(1, 0.6, 0));    //Refracted Sphere
	Sphere *sphere3 = new Sphere(glm::vec3(8.0, 0.0, -85.0), 4.0, glm::vec3(1, 0, 0));      //Textured Sphere
	Sphere *sphere4 = new Sphere(glm::vec3(-12.5, -12.5, -82.5), 4.0, glm::vec3(0.3, 0.7, 0.2));    //Green Sphere

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1);
	sceneObjects.push_back(sphere2);
	sceneObjects.push_back(sphere3);
	sceneObjects.push_back(sphere4);
}


//------ The floor creation function ------------
//    creates a large instance of the plane class
//		to be used in the scene as a 'floor'
//------------------------------------------------
void createFloor()
{
	//-- Create a pointer to a plane object
	Plane *plane = new Plane (glm::vec3(-200.f, -20.f, 200),     //Point A
                            glm::vec3(200.f, -20, 200),    //Point B
                            glm::vec3(200.f, -20, -200),   //Point C
                            glm::vec3(-200.f, -20, -200),  //Point D
                            glm::vec3(1)); //Colour

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(plane);
}


//------- The box creation function ------------
//    creates 6 instances of the plane class
//		which form a box in the scene
//--------------------------------------------
void createBox()
{
	//-- Create a pointer to 6 plane objects
	Plane *bottom = new Plane (glm::vec3(6, -15, -80),     //Point A
								glm::vec3(4, -15, -84),    //Point B
								glm::vec3(9, -15, -84),   //Point C
								glm::vec3(11, -15, -80),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour

	Plane *top = new Plane (glm::vec3(6, -10, -80),     //Point A
								glm::vec3(11, -10, -80),    //Point B
								glm::vec3(9, -10, -84),   //Point C
								glm::vec3(4, -10, -84),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour

	Plane *front = new Plane (glm::vec3(6, -10, -80),     //Point A
								glm::vec3(6, -15, -80),    //Point B
								glm::vec3(11, -15, -80),   //Point C
								glm::vec3(11, -10, -80),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour

	Plane *right = new Plane (glm::vec3(11, -10, -80),     //Point A
								glm::vec3(11, -15, -80),    //Point B
								glm::vec3(9, -15, -84),   //Point C
								glm::vec3(9, -10, -84),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour

	Plane *back = new Plane (glm::vec3(9, -10, -84),     //Point A
								glm::vec3(9, -15, -84),    //Point B
								glm::vec3(4, -15, -84),   //Point C
								glm::vec3(4, -10, -84),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour

	Plane *left = new Plane (glm::vec3(4, -10, -84),     //Point A
								glm::vec3(4, -15, -84),    //Point B
								glm::vec3(6, -15, -80),   //Point C
								glm::vec3(6, -10, -80),  //Point D
								glm::vec3(0.8, 0, 0.8));     //Colour


	//--Add the above to the list of scene objects.
	sceneObjects.push_back(bottom);
	sceneObjects.push_back(top);
	sceneObjects.push_back(front);
	sceneObjects.push_back(right);
	sceneObjects.push_back(back);
	sceneObjects.push_back(left);
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

	createSpheres();   //create the spheres
	createFloor();   //create the plane that is the floor
	createBox();   //create a box using 6 instances of the plane object

	char *s = "Squares.bmp";    //texture file to be used
	texture = TextureBMP(s);
}


//main function that starts the main loop after setting some parameters
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Ray Tracer");
    initialize();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
