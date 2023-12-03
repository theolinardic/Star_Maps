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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <objects.h>

class element
{
public:
	int ui_id, img_width, img_height, status;
	GLuint texture_id, VAO, VBO, EBO, shader;
	float aspect_ratio;
	bool should_render;

	element(int id);
	void switch_img(int new_status);
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
};

class text_element
{
public:
	int text_id;
	std::string text;
	GLuint texture_id, VAO, VBO, shader;
	glm::vec2 position;
	bool should_render;

	text_element(int id, std::string text);
	void render();
};

class HUD
{
public:
	std::vector<element*> all_elements;
	std::vector<text_element*> all_text_elements;

	HUD();
	void add_element(int element_id);
	void add_text_element(int element_id, std::string default_text);
	void update_element(int element_id, int new_status);
	void hide_hud();
	void show_hud();
	void render(glm::vec3 camera_position, glm::vec3 camera_front);
};