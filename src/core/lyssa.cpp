// clang-format off
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include "core/shader.h"

#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_touch.h>
#include <spdlog/spdlog.h>
//glm is used to create perspective and transform matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// OpenCV
#include <opencv2/opencv.hpp>
// std
#include <exception>
#include <stdexcept>
#include <string>

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

void bind_cvmat_to_gl_texture(cv::Mat &image_mat, GLuint &texture)
{
	if(image_mat.empty())
	{
		throw std::runtime_error("Can't load image from: cv::Mat &image_mat");
	}
	else
	{
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glGenTextures(1, &texture);

		// Error check
		GLenum error_gen = glGetError();
		if(error_gen != GL_NO_ERROR)
		{
			std::string _error = "OpenGL error after generation textures: ";
			_error += std::to_string(error_gen);
			throw std::runtime_error(_error);
		}

		glBindTexture(GL_TEXTURE_2D, texture);

		GLenum error_bind = glGetError();
		if(error_bind != GL_NO_ERROR)
		{
			std::string _error = "OpenGL error after bind textures: ";
			_error += std::to_string(error_bind);
			throw std::runtime_error(_error);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// uv are not in [0,1] for some models, they assume uvs are repeated.
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		cv::cvtColor(image_mat, image_mat, cv::COLOR_RGB2BGR);

		glTexImage2D(
			GL_TEXTURE_2D,	  // Type of texture
			0,				  // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,			  // Internal colour format to convert to
			image_mat.cols,	  // Image width  i.e. 640 for Kinect in standard mode
			image_mat.rows,	  // Image height i.e. 480 for Kinect in standard mode
			0,				  // Border width in pixels (can either be 1 or 0)
			GL_RGB,			  // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE, // Image data type
			image_mat.ptr()); // The actual image data itself

		GLenum error_image_2d = glGetError();
		if(error_image_2d != GL_NO_ERROR)
		{
			std::string _error = "OpenGL error after define two-dimensional texture image: ";
			_error += std::to_string(error_image_2d);
			throw std::runtime_error(_error);
		}

		//Random models from the
		// internet come with random formats, and generating mipmaps may be
		// non-trivial.
		glGenerateMipmap(GL_TEXTURE_2D);
	}
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
	// Set some OpenGL settings
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	// we are drawing 3d objects so we want depth testing
	glEnable(GL_DEPTH_TEST);

	// texture handle
	GLuint buffer_texture_with_mat;

	// Shaders
	std::string vert_shader = "#version 330 core\n"
							  "\n"
							  "layout(location = 0) in vec4 position;\n"
							  "layout(location = 1) in vec2 texCoord;\n"
							  "\n"
							  "out vec2 v_TexCoord;\n"
							  "uniform mat4 u_MVP;\n"
							  "\n"
							  "void main() {\n"
							  "	v_TexCoord = texCoord;\n"
							  "	gl_Position = position;\n"
							  "}\n";

	std::string frag_shader = "#version 330 core\n"
							  "\n"
							  "layout(location = 0) out vec4 color;\n"
							  "\n"
							  "in vec2 v_TexCoord;\n"
							  "\n"
							  "uniform sampler2D u_Texture;\n"
							  "\n"
							  "void main()\n"
							  "{\n"
							  "	color = texture(u_Texture, v_TexCoord);\n"
							  "}\n";

	GLuint texture_shader;
	try
	{
		texture_shader = create_program(vert_shader, frag_shader);
	}
	catch(std::exception &e)
	{
		spdlog::error("Error with creating program: {}", e.what());
	}
	glUseProgram(texture_shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	texture texture("test.png");
	texture.bind();
	glUniform1i(glGetUniformLocation(texture_shader, "u_Texture"), 0); // glUseProgram(redP_shader);

	//Scaling the Verts:
	float *scale = texture.get_scale_normalized();

	// Verts
	float h = scale[1], w = scale[0];
	float positions[] = {
		-w,
		-h,
		0.0,
		0.0,
		-w,
		h,
		0.0,
		1.0,
		w,
		-h,
		1.0,
		0.0,
		//
		w,
		h,
		1.0,
		1.0};

	// float texture_position_1[] = {
	// 	-w,
	// 	-h,
	// 	0.0,
	// 	0.0,
	// 	-w,
	// 	h,
	// 	0.0,
	// 	1.0,
	// 	w,
	// 	-h,
	// 	1.0,
	// 	0.0,
	// 	//
	// 	w,
	// 	h,
	// 	1.0,
	// 	1.0};

	GLuint indices[] {0, 1, 2, 1, 2, 3};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (char *)0 + 0 * sizeof(GLfloat));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (char *)0 + 2 * sizeof(GLfloat));

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	bool exit = false;

	SDL_Event event;

	while(!exit)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					// exit = true;
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							exit = true;
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
					break;
				case SDL_QUIT:
					exit = true;
					break;
				default:
					break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// // Texture
		// try
		// {
		// 	cv::Mat image = cv::imread("test.png");

		// 	bind_cvmat_to_gl_texture(image, buffer_texture_with_mat);
		// }
		// catch(std::exception &e)
		// {
		// 	spdlog::error("Error: {}", e.what());
		// }

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// check for errors
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			spdlog::error("{}", error);
			break;
		}

		SDL_GL_SwapWindow(window);
	}

	glDeleteTextures(1, &buffer_texture_with_mat);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}