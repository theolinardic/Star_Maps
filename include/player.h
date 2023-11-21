#pragma once

#include <iostream>
#include <fstream>
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
#include <renderer.h>


#define PLANET_NAME_JSON "assets/planet_names.json"
#define SETTINGS_FILE_LOCATION "settings.json"
extern float movement_speed;
extern float sensitivity;
extern bool show_debug;

glm::vec3 read_player_input(GLFWwindow* star_maps_window, glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch);
nlohmann::json get_settings();
void change_setting(const std::string setting, const nlohmann::json new_value);
void reset_settings();
nlohmann::json generate_new_save_data(const std::string difficulty, const std::string galaxy_name);
void create_new_save(const std::string difficulty, const std::string galaxy_name);
void delete_save(int delete_index);
std::vector<nlohmann::json> preview_saves();
nlohmann::json load_save_file(int index);
void update_save(int index, const nlohmann::json& new_data);
