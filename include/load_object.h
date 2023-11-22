#ifndef LOADOJECT_H
#define LOADOJECT_H

// loadObject serves to load local object files and return them back ready
// to be rendered.

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

// Constant definitions:
#define PLANET_OBJ_FILE_LOC "assets/objects/planets/planet.obj"

// Function declarations:

class planet
{
public:
	std::string name;
	//float distance_from_sun;
	//float orbit_speed;
	//float rotation_speed;
	bool visible;
	GLuint texture_id;

	planet(std::string planet_name, const char* file_name);
	void print_name();
	void render(GLuint vao, GLuint planet_shader);
	void load_texture(const char* file_name);
};

int load_object_file(char* file_name, std::vector <glm::vec3>& v_out,
	std::vector <glm::vec2>& uv_out, std::vector <glm::vec3>& n_out);


GLchar* read_shader_text(const char* file_path);
GLuint load_shader(const char* vertex_file, const char* frag_file);


#endif