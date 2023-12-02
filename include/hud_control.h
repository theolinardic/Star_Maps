#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include <objects.h>

class HUD
{
public:
	GLuint tiles_texture_id, tools_texture_id, progress_bars_texture_id, time_bar_texture_id, VAO, VBO, EBO, shader;

	HUD();
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
};