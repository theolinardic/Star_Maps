// loadShader serves to load local shaders and return them back ready for
// OpenGL.

#include <iostream>
#include <fstream>
#include <streambuf>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>


// Constant definitions:

// Function declarations:
GLchar* read_shader_text(const char* file_path);
GLuint load_shader(const char* vertex_file, const char* frag_file);
