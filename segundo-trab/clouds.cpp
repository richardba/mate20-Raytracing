#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#ifdef ant
#include <AntTweakBar.h>
#endif // ant
//#define RAD
using namespace glm;
using namespace std;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#define WINDOW_H 1024
#define WINDOW_W 768

const glm::vec4 clearColor(0.53f, 0.9f, 1.f, 0.0f);

float inrand(float Min, float Max)
{
  return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}


struct Particle
{
	glm::vec3 pos, speed;
	unsigned char r,g,b,a;
	float size, angle, weight;
	float life;
	float cameradistance;

	bool operator<(const Particle& that) const
	{
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100000;
const GLfloat distribution = 1000.0f;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

int FindUnusedParticle(){

	for(int i=LastUsedParticle; i<MaxParticles; i++)
  {
		if (ParticlesContainer[i].life < 0)
		{
			LastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<LastUsedParticle; i++)
  {
		if (ParticlesContainer[i].life < 0)
		{
			LastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void SortParticles()
{
	sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}
#ifdef ant
inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
{TwEventMouseButtonGLFW(button, action);}
inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
{TwMouseMotion(int(xpos), int(ypos));}
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset)
{TwEventMouseWheelGLFW(yoffset);}
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods)
{TwEventKeyGLFW(key, action);}
inline void TwEventCharGLFW3(GLFWwindow* window, int codepoint)
{TwEventCharGLFW(codepoint, GLFW_PRESS);}
inline void TwWindowSizeGLFW3(GLFWwindow* window, int width, int height)
{TwWindowSize(width, height);}
#endif // ant

int main( int argv, char** argc )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		cout << "Failed to initialize GLFW" << endl;
		cin >> argv;
		return EXIT_FAILURE;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( WINDOW_H, WINDOW_W, "Cloud simulation", NULL, NULL);
	if( window == NULL )
  {
		cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible." << endl;
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
  {
		cout << "Failed to initialize GLEW" << endl;
		cin >> argv;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_H/2, WINDOW_W/2);

#ifdef ant
  if(!TwInit(TW_OPENGL_CORE, NULL))
  {
    cout << "Failed to initialize AntTweakBar" << endl;
		cin >> argv;
		return EXIT_FAILURE;
  }
  TwWindowSize(WINDOW_W, WINDOW_H);
  TwBar *myBar;
  myBar = TwNewBar("Acceleration");
  TwDefine("Acceleration label='o/l color -/+ speed' position='10 280' size='240 100' alpha=64 help=' ' ");

    glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);

    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
#endif // ant
	glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint programID = LoadShaders( "Particle.vert", "Particle.frag" );
	GLuint programAtmosphere = LoadShaders( "Rayleigh.vert", "Rayleigh.frag" );

	GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");




	static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];

	for(int i=0; i<MaxParticles; i++)
  {
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}

	GLuint tex = loadDDS("particle.DDS");
//	GLuint tex = noiseTexture();

  static const GLfloat g_atmosphere_data[] =
  {
    -1, -1, -1,
     1, -1, -1,
     1,  1, -1,
    -1, -1, -1,
     1,  1, -1,
    -1,  1, -1
  };
	static const GLfloat g_vertex_buffer_data[] =
	{
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

  GLuint atmosphere_vertex_buffer;
	glGenBuffers(1, &atmosphere_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, atmosphere_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_atmosphere_data), g_atmosphere_data, GL_STATIC_DRAW);

  float acceleration = 1.f;
  GLuint threshold = 192;

#ifdef ant
  TwAddVarRO(myBar, "Acceleration", TW_TYPE_FLOAT, &acceleration, "");
  TwAddVarRO(myBar, "Threshold", TW_TYPE_INT16, &threshold, "");
#endif // ant

  bool use=false;
	double lastTime = glfwGetTime();
	do
	{
    if (glfwGetKey( window, GLFW_KEY_EQUAL) == GLFW_PRESS && acceleration<=2)
    {
      acceleration+=0.05f;
    }
    if (glfwGetKey( window, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
      acceleration/=2.f;
    }
    if (glfwGetKey( window, GLFW_KEY_O) == GLFW_PRESS && threshold<255)
    {
      threshold+=1;
    }
    if (glfwGetKey( window, GLFW_KEY_L) == GLFW_PRESS && threshold>0)
    {
      threshold-=1;
    }
    GLuint Texture = tex;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//Atmosphere
		glUseProgram(programAtmosphere);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, atmosphere_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;


		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);

		for(int i=0; i<newparticles; i++)
    {
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 5.0f;
			ParticlesContainer[particleIndex].pos = glm::vec3(0,0,-20.0f);

			float spread = 3.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 0.0f, 10.0f);
    //Distribuição radial
    #ifdef RAD
      GLfloat phi = inrand(0,2*M_PI);
      GLfloat cosTheta = inrand(-1,1);
      GLfloat u = inrand(0,1);
      GLfloat theta = acos(cosTheta);
      GLfloat r = (float)distribution*cbrt(u);

      glm::vec3 randomdir = glm::vec3(
        r * sin( theta) * cos( phi ),
        r * sin( theta) * sin( phi ),
        r * cos( theta )
      );
    #else
    //Distribuição linear
      glm::vec3 randomdir = glm::vec3(
				(rand()%(int)(2*distribution) - distribution)/distribution,
				(rand()%(int)(2*distribution) - distribution)/distribution,
				(rand()%(int)(2*distribution) - distribution)/distribution
			);

    #endif // RAD
			ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;

      unsigned char w = inrand(threshold,256);
			ParticlesContainer[particleIndex].r = w;
			ParticlesContainer[particleIndex].g = w;
			ParticlesContainer[particleIndex].b = w;
			ParticlesContainer[particleIndex].a = (rand() % 256) / 4;

			ParticlesContainer[particleIndex].size = (rand()%(int)distribution)/(2*distribution) + inrand(0,1);
		}

		int ParticlesCount = 0;
		for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i];

			if(p.life > 0.0f)
      {

				p.life -= delta;
				if (p.life > 0.0f)
        {
					p.speed += glm::vec3(0.f, 0.f, 9.f*acceleration) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2( p.pos - CameraPosition );

					g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
					g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
					g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;

					g_particule_position_size_data[4*ParticlesCount+3] = p.size;

					g_particule_color_data[4*ParticlesCount+0] = p.r;
					g_particule_color_data[4*ParticlesCount+1] = p.g;
					g_particule_color_data[4*ParticlesCount+2] = p.b;
					g_particule_color_data[4*ParticlesCount+3] = p.a;

				}
				else
				{
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

		SortParticles();

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


		glUseProgram(programID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			2,
			4,
			GL_UNSIGNED_BYTE,
			GL_TRUE,
			0,
			(void*)0
		);

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
		#ifdef ant
    TwDraw();
    #endif // ant
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

    //End of atmosphere
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] g_particule_position_size_data;

	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &tex);
	glDeleteVertexArrays(1, &VertexArrayID);

  #ifdef ant
	TwTerminate();
  #endif // ant
	glfwTerminate();

	return 0;
}

