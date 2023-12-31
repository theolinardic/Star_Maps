#pragma once

#include <random>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include <sstream>
#include <json.hpp>
#include <objects.h>
#include <hud_control.h>

#define SETTINGS_FILE_LOCATION "settings.json"

extern float movement_speed;
extern float sensitivity;
extern bool show_debug;
#define PLANET_NAME_JSON "assets/planet_names.json"
using json = nlohmann::json;

class star_maps_game
{
public:
	float game_speed_multiplier, player_money;
	bool paused, loaded_save;
	int num_entitys, current_save_idx;
	std::vector<game_object*> entitiys;
	rings* orbit_rings;
	std::string current_save_name;
	int in_game_hour, in_game_minute, sols_passed;
	float in_game_second;
	int total_played_hour, total_played_minute;
	float total_played_second;
	int difficulty; // 1 = Easy, 2 = Medium, 3 = Hard.
	float status_1_percent, status_2_percent, status_3_percent;
	GLFWwindow* window;
	HUD* game_ui;
	// 0 = none, 1 = left, 2 = right
	int last_frame_down;
	bool in_menu;
	bool spawned_recently;
	long int daily_cost;
	long int daily_gain;

	int current_tool, placing;

	star_maps_game(bool p, GLFWwindow* window, HUD* ui);
	void reset_settings();
	json get_settings();
	void change_setting(const std::string setting, const json new_value);
	void load_save(int index);
	void save_game();
	json generate_new_save_data(const std::string difficulty, const std::string galaxy_name);
	void create_new_save(const std::string difficulty, const std::string galaxy_name);
	void delete_save(int delete_index);
	std::vector<json> preview_saves();
	void update_save(int index, const json& new_data);
	void give_money(int amount);
	void update_ingame_clock(float time_to_add, HUD* game_ui);
	void entity_manager(const glm::vec3& camera_position, const glm::vec3& camera_front);
	void spawn_entity(int type, int texture_id, int parent_in, glm::vec3 location, bool npc);
	void despawn_entity(int entity_id);
	void despawn_all_entities();
	void switch_camera_mode(glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, int view_switch);
	void save_and_return_to_menu();
	void close_game();
	void read_player_input(glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch);
	bool purchase(int cost);
};

int diff_text_to_int(std::string diff);
std::string diff_int_to_text(int diff);

void switch_display_type(GLFWwindow* window, bool fullscreen);