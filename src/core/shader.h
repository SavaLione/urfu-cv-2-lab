#ifndef CORE_SHADER_H
#define CORE_SHADER_H

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

#include <string>

GLuint load_shader(std::string vertex_file_path, std::string fragment_file_path);
std::string read_file(std::string file_name);

#endif // CORE_SHADER_H