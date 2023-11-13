// loadShader serves to load local shaders and return them back ready for
// OpenGL.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Constant definitions:

// Function declarations:
void load_shader(const char* vertex_file, const char* frag_file);