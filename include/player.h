#pragma once

#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <objects.h>

#define PLANET_NAME_JSON "assets/planet_names.json"

// Make movement speed, sensitivity, and show debug global variables so any class
// can easily access them:
extern float movement_speed;
extern float sensitivity;
extern bool show_debug;

void read_player_input(GLFWwindow* star_maps_window, glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, std::vector<game_object*> entitiys);
