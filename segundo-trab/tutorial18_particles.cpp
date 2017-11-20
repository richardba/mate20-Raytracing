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

#include <AntTweakBar.h>
using namespace glm;
using namespace std;

#include "../common/shader.hpp"
#include "../common/texture.hpp"
#include "../common/controls.hpp"
#define WINDOW_H 1024
#define WINDOW_W 768

float inrand(float Min, float Max)
{
  return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100000;
const GLfloat distribution = 1000.0f;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

	for(int i=LastUsedParticle; i<MaxParticles; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<LastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles(){
	sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( WINDOW_H, WINDOW_W, "Tutorial 18 - Particules", NULL, NULL);
	if( window == NULL )
  {
		cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible." << endl;
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
  {
		cout << "Failed to initialize GLEW" << endl;
		cin >> argv;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the mouse at the center of the screen
  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_H/2, WINDOW_W/2);
  if(!TwInit(TW_OPENGL_CORE, NULL))
  {
    cout << "Failed to initialize AntTweakBar" << endl;
		cin >> argv;
		return EXIT_FAILURE;
  }
  TwWindowSize(WINDOW_W, WINDOW_H);
  TwBar *myBar;
  myBar = TwNewBar("Acceleration");
  TwDefine("Acceleration label='Clouds Inputs' position='10 280' size='240 100' alpha=64 help=' ' ");
    glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
	// Dark blue background
	glClearColor(0.53f, 0.9f, 1.f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "Particle.vert", "Particle.frag" );

	// Vertex shader
	GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

	// fragment shader
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");


	static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];

	for(int i=0; i<MaxParticles; i++)
  {
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}



	GLuint tex = loadDDS("particle.DDS");

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);


  float acceleration = 1.f;
  GLuint threshold = 192;

  TwAddVarRO(myBar, "Acceleration", TW_TYPE_FLOAT, &acceleration, "");
  TwAddVarRO(myBar, "Threshold", TW_TYPE_INT16, &threshold, "");
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
    //GLuint Texture = tex*rand();
    GLuint Texture = tex;
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;


		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		// We will need the camera's position in order to sort the particles
		// w.r.t the camera's distance.
		// There should be a getCameraPosition() function in common/controls.cpp,
		// but this works too.
		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


		// Generate 10 new particule each millisecond,
		// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
		// newparticles will be huge and the next frame even longer.
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);

		for(int i=0; i<newparticles; i++)
    {
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = glm::vec3(0,0,-20.0f);

			float spread = 3.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 0.0f, 10.0f);

//			{
//        GLfloat phi = inrand(0,2*M_PI);
//        GLfloat cosTheta = inrand(-1,1);
//        GLfloat u = inrand(0,1);
//        GLfloat theta = acos(cosTheta);
//        GLfloat r = (float)distribution*cbrt(u);
//
//        //Spherical Distribution
//        glm::vec3 randomdir = glm::vec3(
//          r * sin( theta) * cos( phi ),
//          r * sin( theta) * sin( phi ),
//          r * cos( theta )
//        );
//      }

      glm::vec3 randomdir = glm::vec3(
				(rand()%(int)(2*distribution) - distribution)/distribution,
				(rand()%(int)(2*distribution) - distribution)/distribution,
				(rand()%(int)(2*distribution) - distribution)/distribution
			);

			ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;

      unsigned char w = inrand(threshold,256);
			// Very bad way to generate a random color
			ParticlesContainer[particleIndex].r = w;
			ParticlesContainer[particleIndex].g = w;
			ParticlesContainer[particleIndex].b = w;
			ParticlesContainer[particleIndex].a = (rand() % 256) / 4;

			ParticlesContainer[particleIndex].size = (rand()%(int)distribution)/(2*distribution) + inrand(0,1);

		}

		int ParticlesCount = 0;
		for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if(p.life > 0.0f){

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f)
        {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.f, 0.f, 9.f*acceleration) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2( p.pos - CameraPosition );
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
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
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

		SortParticles();

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader

		glUseProgram(programID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		// The first parameter is the attribute buffer we're talking about.
		// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

		// Draw the particules !
		// This draws many times a small triangle_strip (which looks like a quad).
		// This is equivalent to :
		// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
		// but faster.
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
    TwDraw();
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] g_particule_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &tex);
	glDeleteVertexArrays(1, &VertexArrayID);


	// Close OpenGL window and terminate GLFW
	TwTerminate();
	glfwTerminate();

	return 0;
}

