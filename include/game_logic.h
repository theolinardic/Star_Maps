#pragma once

#include <iostream>
#include <fstream>
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
#include <json.hpp>
#include <renderer.h>
#include <rings.h>
#include <player.h>

class star_maps_game
{
public:
	float game_speed_multiplier;
	bool paused;
	bool loaded_save;

	long int num_entitys;
	std::vector<game_object*> entitiys;
	rings* orbit_rings;

	int current_save_idx;
	std::string current_save_name;
	float player_money;
	int sols_passed;
	int in_game_hour;
	int in_game_minute;
	float in_game_second;
	int total_played_hour;
	int total_played_minute;
	float total_played_second;
	//1=easy, 2=medium, 3=hard
	int difficulty;
	float status_1_percent;
	float status_2_percent;
	float status_3_percent;


	star_maps_game(bool p);
	void update_debug_ingame_clock(float time_to_add);
	void give_money(int amount);
	void load_save(int index);
	void save_game();
	void entity_manager(const glm::vec3& cameraPosition, const glm::vec3& cameraFront, float game_speed);
	void spawn_entity(int type, int texture_id, int parent_in, glm::vec3 location);
	void despawn_entity(int entity_id);
	void close_game();
	void despawn_all_entities();
	void check_click(glm::vec3 camera_position, glm::vec3 mouse_pos);
};
