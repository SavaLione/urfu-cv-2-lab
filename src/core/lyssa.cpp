// clang-format off
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include "core/shader.h"

#include <SDL_keycode.h>
#include <spdlog/spdlog.h>
//glm is used to create perspective and transform matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj)
{
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		spdlog::error("{}", &log[0]);
		return false;
	}
	return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj)
{
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		spdlog::error("{}", &log[0]);
		return false;
	}
	return true;
}

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
	// we are drawing 3d objects so we want depth testing
	glEnable(GL_DEPTH_TEST);

	// shader source code
	std::string vertex_source = "#version 330\n"
								"uniform mat4 ViewProjection;\n"		  // the projection matrix uniform
								"uniform samplerBuffer offset_texture;\n" // the buffer_texture sampler
								"layout(location = 0) in vec4 vposition;\n"
								"layout(location = 1) in vec4 vcolor;\n"
								"out vec4 fcolor;\n"
								"void main() {\n"
								// access the buffer texture with the InstanceID (tbo[InstanceID])
								"   vec4 offset = texelFetch(offset_texture, gl_InstanceID);\n"
								"   fcolor = vcolor;\n"
								"   gl_Position = ViewProjection*(vposition + offset);\n"
								"}\n";

	std::string fragment_source = "#version 330\n"
								  "in vec4 fcolor;\n"
								  "layout(location = 0) out vec4 FragColor;\n"
								  "void main() {\n"
								  "   FragColor = fcolor;\n"
								  "}\n";

	// program and shader handles
	GLuint shader_program, vertex_shader, fragment_shader;

	// we need these to properly pass the strings
	const char *source;
	int length;

	// create and compiler vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source		  = vertex_source.c_str();
	length		  = vertex_source.size();
	glShaderSource(vertex_shader, 1, &source, &length);
	glCompileShader(vertex_shader);
	if(!check_shader_compile_status(vertex_shader))
	{
		return -1;
	}

	// create and compiler fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source			= fragment_source.c_str();
	length			= fragment_source.size();
	glShaderSource(fragment_shader, 1, &source, &length);
	glCompileShader(fragment_shader);
	if(!check_shader_compile_status(fragment_shader))
	{
		return -1;
	}

	// create program
	shader_program = glCreateProgram();

	// attach shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	// link the program and check for errors
	glLinkProgram(shader_program);
	check_program_link_status(shader_program);

	// obtain location of projection uniform
	GLint ViewProjection_location = glGetUniformLocation(shader_program, "ViewProjection");
	GLint offset_texture_location = glGetUniformLocation(shader_program, "offset_texture");

	// vao and vbo handles
	GLuint vao, vbo, tbo, ibo;

	// generate and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// generate and bind the vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// clang-format off
    // data for a cube
    GLfloat vertexData[] = {
    //  X     Y     Z           R     G     B
    // face 0:
       1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f, // vertex 0
      -1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f, // vertex 1
       1.0f,-1.0f, 1.0f,       1.0f, 0.0f, 0.0f, // vertex 2
      -1.0f,-1.0f, 1.0f,       1.0f, 0.0f, 0.0f, // vertex 3
 
    // face 1:
       1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f, // vertex 0
       1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f, // vertex 1
       1.0f, 1.0f,-1.0f,       0.0f, 1.0f, 0.0f, // vertex 2
       1.0f,-1.0f,-1.0f,       0.0f, 1.0f, 0.0f, // vertex 3
 
    // face 2:
       1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f, // vertex 0
       1.0f, 1.0f,-1.0f,       0.0f, 0.0f, 1.0f, // vertex 1
      -1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f, // vertex 2
      -1.0f, 1.0f,-1.0f,       0.0f, 0.0f, 1.0f, // vertex 3
      
    // face 3:
       1.0f, 1.0f,-1.0f,       1.0f, 1.0f, 0.0f, // vertex 0
       1.0f,-1.0f,-1.0f,       1.0f, 1.0f, 0.0f, // vertex 1
      -1.0f, 1.0f,-1.0f,       1.0f, 1.0f, 0.0f, // vertex 2
      -1.0f,-1.0f,-1.0f,       1.0f, 1.0f, 0.0f, // vertex 3
 
    // face 4:
      -1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f, // vertex 0
      -1.0f, 1.0f,-1.0f,       0.0f, 1.0f, 1.0f, // vertex 1
      -1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 1.0f, // vertex 2
      -1.0f,-1.0f,-1.0f,       0.0f, 1.0f, 1.0f, // vertex 3
 
    // face 5:
       1.0f,-1.0f, 1.0f,       1.0f, 0.0f, 1.0f, // vertex 0
      -1.0f,-1.0f, 1.0f,       1.0f, 0.0f, 1.0f, // vertex 1
       1.0f,-1.0f,-1.0f,       1.0f, 0.0f, 1.0f, // vertex 2
      -1.0f,-1.0f,-1.0f,       1.0f, 0.0f, 1.0f, // vertex 3
    }; // 6 faces with 4 vertices with 6 components (floats)
	// clang-format on

	// generate and bind the index buffer object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	// clang-format off
    GLuint indexData[] = {
        // face 0:
        0,1,2,      // first triangle
        2,1,3,      // second triangle
        // face 1:
        4,5,6,      // first triangle
        6,5,7,      // second triangle
        // face 2:
        8,9,10,     // first triangle
        10,9,11,    // second triangle
        // face 3:
        12,13,14,   // first triangle
        14,13,15,   // second triangle
        // face 4:
        16,17,18,   // first triangle
        18,17,19,   // second triangle
        // face 5:
        20,21,22,   // first triangle
        22,21,23,   // second triangle
    };
	// clang-format on

	// fill with data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6 * 2 * 3, indexData, GL_STATIC_DRAW);

	// generate and bind the buffer object containing the
	// instance offsets
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);

	// clang-format off
    // the offsets
    GLfloat translationData[] = {
                 2.0f, 2.0f, 2.0f, 0.0f,  // cube 0
                 2.0f, 2.0f,-2.0f, 0.0f,  // cube 1
                 2.0f,-2.0f, 2.0f, 0.0f,  // cube 2
                 2.0f,-2.0f,-2.0f, 0.0f,  // cube 3
                -2.0f, 2.0f, 2.0f, 0.0f,  // cube 4
                -2.0f, 2.0f,-2.0f, 0.0f,  // cube 5
                -2.0f,-2.0f, 2.0f, 0.0f,  // cube 6
                -2.0f,-2.0f,-2.0f, 0.0f,  // cube 7
    }; // 8 offsets with 3 components each
	// clang-format on

	// fill with data
	glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat) * 4 * 8, translationData, GL_STATIC_DRAW);

	// texture handle
	GLuint buffer_texture;

	// generate and bind the buffer texture
	glGenTextures(1, &buffer_texture);
	glBindTexture(GL_TEXTURE_BUFFER, buffer_texture);

	// tell the buffer texture to use
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tbo);

	// fill with data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4 * 6, vertexData, GL_STATIC_DRAW);

	// set up generic attrib pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (char *)0 + 0 * sizeof(GLfloat));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (char *)0 + 3 * sizeof(GLfloat));

	float t = 0;

	bool exit = false;

	// SDL_Event event;

	while(!exit)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					// exit = true;
					switch(event.key.keysym.sym)
					{
						case SDLK_RIGHT:
							t += 0.1;
							break;
						case SDLK_LEFT:
							t -= 0.1;
							break;
						case SDLK_ESCAPE:
							exit = true;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					exit = true;
					break;
				default:
					break;
			}
		}

		// get the time in seconds
		// float t = glfwGetTime();
		// float t = (SDL_GetTicks() * 1000);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use the shader program
		glUseProgram(shader_program);

		// calculate ViewProjection matrix
		glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.f);

		// translate the world/view position
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

		// make the camera rotate around the origin
		View = glm::rotate(View, 90.0f * t, glm::vec3(1.0f, 1.0f, 1.0f));

		glm::mat4 ViewProjection = Projection * View;

		// bind texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, buffer_texture);

		// set the matrix uniform
		glUniformMatrix4fv(ViewProjection_location, 1, GL_FALSE, glm::value_ptr(ViewProjection));

		// set texture uniform
		glUniform1i(offset_texture_location, 0);

		// bind the vao
		glBindVertexArray(vao);

		// draw
		// the additional parameter indicates how many instances to render
		glDrawElementsInstanced(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0, 8);

		// check for errors
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			spdlog::error("{}", error);
			break;
		}

		SDL_GL_SwapWindow(window);
	}

	// delete the created objects
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &tbo);

	glDeleteTextures(1, &buffer_texture);

	glDetachShader(shader_program, vertex_shader);
	glDetachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shader_program);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}