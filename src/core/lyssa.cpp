// clang-format off
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include "core/shader.h"

#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		spdlog::error("Error: {}", SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char *glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_Window *window = SDL_CreateWindow("OpenGL Test", 0, 0, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(!window)
	{
		spdlog::error("Error: {}", SDL_GetError());
		return -1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if(!context)
	{
		spdlog::error("Error: {}", SDL_GetError());
		return -1;
	}

	// Enable glew experimental, this enables some more OpenGL extensions.
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK)
	{
		spdlog::error("Failed to initialize GLEW");
		return -1;
	}

	// Application Variables
	GLfloat angle_pyramid = 0.0f;
	GLfloat angle_cube	  = 0.0f;

	// Set some OpenGL settings
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLfloat aspect = (GLfloat)800 / (GLfloat)800;
	glViewport(0, 0, 800, 800);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);

	bool exit = false;

	SDL_Event event;

	while(!exit)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					exit = true;
					break;
				case SDL_QUIT:
					exit = true;
					break;
				default:
					break;
			}
		}

		// Rotate the shapes
		angle_pyramid += 0.2f;
		angle_cube -= 0.15f;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);

		// Reset the current transform
		glLoadIdentity();

		// Set the cube location
		glTranslatef(1.5f, 0.0f, -7.0f);
		glRotatef(angle_cube, 1.0f, 1.0f, 1.0f);

		// Draw the cube
		glBegin(GL_QUADS);
		{
			// Top face (y = 1.0f)
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);

			// Bottom face (y = -1.0f)
			glColor3f(1.0f, 0.5f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);

			// Front face  (z = 1.0f)
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);

			// Back face (z = -1.0f)
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);

			// Left face (x = -1.0f)
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);

			// Right face (x = 1.0f)
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
		}
		glEnd();

		// Reset the current transform
		glLoadIdentity();

		// Set the pyramid location
		glTranslatef(-1.5f, 0.0f, -6.0f);
		glRotatef(angle_pyramid, 1.0f, 1.0f, 0.0f);

		// Draw the pyramid
		glBegin(GL_TRIANGLES);
		{
			// Front
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);

			// Right
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);

			// Back
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);

			// Left
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
		}
		glEnd();

		//glViewport(0, 0, 800, 800);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}