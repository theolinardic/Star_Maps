#pragma once

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <load_object.h>

// Constant definitions:
#define WINDOW_NAME "Star Maps - v0.01"

// Function declarations:
GLFWwindow* initialize_glfw_window(void);
GLuint load_cubemap(const char* faces[]);

class skybox
{
public:
	GLuint vao, vbo, ebo;
	GLuint shader;
	GLuint textureID;
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
	skybox();
};