#ifndef CORE_SHADER_H
#define CORE_SHADER_H

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

#include <string>

GLuint load_shader(std::string vertex_file_path, std::string fragment_file_path);
std::string read_file(std::string file_name);

GLuint compile_shader(unsigned int type, const std::string &source);
GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader);

class texture
{
private:
	GLuint rendererId;
	std::string filePath;
	unsigned char *localBuffer;
	int width, height, BPP;

public:
	texture(const std::string path);
	~texture()
	{
		glDeleteTextures(1, &rendererId);
	}

	void bind(GLuint slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, rendererId);
	}
	void unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Side : res[0] = width; res[1] = height;
	float *get_scale_normalized()
	{
		float *res = (float *)malloc(2 * sizeof(float));
		if(this->width > this->height)
		{
			res[0] = 1;
			res[1] = (float)height / (float)width;
		}
		else
		{
			res[1] = 1;
			res[0] = (float)width / (float)height;
		}
		return res;
	}
};

#endif // CORE_SHADER_H