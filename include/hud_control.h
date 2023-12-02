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

class element
{
public:
	int ui_id;
	GLuint texture_id, VAO, VBO, EBO, shader;
	float aspect_ratio;
	bool should_render;

	element(int id);
	void switch_img(int new_status);
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
};

class HUD
{
public:
	std::vector<element*> all_elements;
	bool should_render;

	HUD();
	void add_element(int element_id);
	void update_element(int element_id, int new_status);
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
};