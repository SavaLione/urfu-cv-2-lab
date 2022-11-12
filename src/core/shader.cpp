#include "core/shader.h"

#include <SDL2/SDL.h>
#include <exception>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "core/stb_image.h"

std::string read_file(std::string file_name)
{
	SDL_RWops *file = SDL_RWFromFile(file_name.c_str(), "rb");
	size_t size;

	if(!file)
	{
		throw std::runtime_error("Failed opening file: " + file_name);
	}

	void *loaded = SDL_LoadFile_RW(file, &size, 1);

	if(!loaded)
	{
		throw std::runtime_error("Failed loading file: " + file_name);
	}

	std::string ret {static_cast<char *>(loaded), size};

	SDL_free(loaded);

	return ret;
}

GLuint load_shader(std::string vertex_file_path, std::string fragment_file_path)
{
	// shaders
	std::string vertex_shader;
	std::string fragment_shader;

	try
	{
		vertex_shader	= read_file("simple_vertex_shader.vert");
		fragment_shader = read_file("simple_fragment_shader.frag");
	}
	catch(std::exception &e)
	{
		spdlog::error("Error: {}", e.what());
		return NULL;
	}

	// States
	GLint result = GL_FALSE;
	int info_log_length;

	// Create the shaders
	GLuint vertex_shader_id	  = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile vertex shader
	char const *vertex_source_pointer = vertex_shader.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
	glCompileShader(vertex_shader_id);

	// Check vertex shader
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

	if(info_log_length > 0)
	{
		std::vector<char> vertex_shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
		spdlog::error("{}", &vertex_shader_error_message[0]);
		return NULL;
	}

	// Compile fragment shader
	char const *fragment_source_pointer = fragment_shader.c_str();
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
	glCompileShader(fragment_shader_id);

	// Check fragment shader
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

	if(info_log_length > 0)
	{
		std::vector<char> fragment_shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
		spdlog::error("{}", &fragment_shader_error_message[0]);
		return NULL;
	}

	// Link the program
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	// Check the program
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if(info_log_length > 0)
	{
		std::vector<char> program_error_message(info_log_length + 1);
		glGetProgramInfoLog(program_id, info_log_length, NULL, &program_error_message[0]);
		spdlog::error("{}", &program_error_message[0]);
		return NULL;
	}

	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

GLuint compile_shader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)malloc(length);
		glGetShaderInfoLog(id, length, &length, message);
		throw std::runtime_error("Compile shader error");
		glDeleteShader(id);
		return 0;
	}
	return id;
}

GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs		 = compile_shader(GL_VERTEX_SHADER, vertex_shader);
	unsigned int fs		 = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

texture::texture(const std::string path)
{
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4);

	glGenTextures(1, &rendererId);
	glBindTexture(GL_TEXTURE_2D, rendererId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(localBuffer)
	{
		stbi_image_free(localBuffer);
	}
}