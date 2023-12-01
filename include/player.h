#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>
#include <vector>
#include <algorithm>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>
#include <game_logic.h>
#include <objects.h>

#define PLANET_NAME_JSON "assets/planet_names.json"

// Make movement speed, sensitivity, and show debug global variables so any class
// can easily access them:
extern float movement_speed;
extern float sensitivity;
extern bool show_debug;

using json = nlohmann::json;

void read_player_input(GLFWwindow* star_maps_window, glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, std::vector<game_object*> entitiys);
json generate_new_save_data(const std::string difficulty, const std::string galaxy_name);
void create_new_save(const std::string difficulty, const std::string galaxy_name);
void delete_save(int delete_index);
std::vector<json> preview_saves();
void update_save(int index, const json& new_data);
