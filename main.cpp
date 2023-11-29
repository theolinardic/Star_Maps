#pragma once

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>
// Below  are original source files for Star Maps:
#include <objects.h>
#include <generate_map.h>
#include <player.h>
#include <game_logic.h>
#include <debug_menu.h>


int main()
{
	// Initialize GLFW window using function from generateMap.cpp.
	GLFWwindow* star_maps_window = initialize_glfw_window();
	glEnable(GL_DEPTH_TEST);
	glfwSwapBuffers(star_maps_window);

	// Create skybox:
	skybox SB = skybox();

	// Set up camera variables:
	glm::vec3 camera_position = glm::vec3(642.0f, 692.0f, -1220.0f);
	glm::vec3 camera_front = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	float camera_yaw = 115.0f;
	float camera_pitch = -35.0f;
	glm::vec3 front;

	
	float launch_time = glfwGetTime();
	float last_frame_time = glfwGetTime();
	float awake_time = 0;

	// Start the brain of the game with the core logic.
	star_maps_game star_maps = star_maps_game(false);

	// Initialize debug menu and add base debug items.
	debug* debug_menu = new debug;
	debug_menu->start();
	debug_menu->add_text_float("Cam Pos X: ", &camera_position.x);
	debug_menu->add_text_float("Cam Pos Y: ", &camera_position.y);
	debug_menu->add_text_float("Cam Pos Z: ", &camera_position.z);
	debug_menu->add_text_float("Cam Rot X: ", &front.x);
	debug_menu->add_text_float("Cam Rot Y: ", &front.y);
	debug_menu->add_text_float("Cam Rot Z: ", &front.z);
	debug_menu->add_checkbox("Paused: ", &star_maps.paused);

	
	// Main game/render loop:
	while (!glfwWindowShouldClose(star_maps_window))
	{
		if (!star_maps.paused)
		{
			awake_time += (glfwGetTime() - last_frame_time);
		}
		// Tells GLFW to check for new events and process them.
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Aadd to the in game counter to increase sol/clock via debug menu in game logic class.
		star_maps.update_debug_ingame_clock(glfwGetTime() - last_frame_time);
		last_frame_time = glfwGetTime();

		front.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		front.y = sin(glm::radians(camera_pitch));
		front.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		camera_front = glm::normalize(front);

		SB.render(camera_position, camera_front);
		star_maps.entity_manager(camera_position, camera_front, star_maps.game_speed_multiplier);

		glm::vec3 check_item = read_player_input(star_maps_window, camera_position, camera_front, camera_yaw, camera_pitch, star_maps.entitiys);
	
		debug_menu->render();

		glfwSwapBuffers(star_maps_window);
	}

	// ImGui Dev UI and GLFW Clean Up at the end of program.
	//ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
	glfwDestroyWindow(star_maps_window);
	glfwTerminate();

	return 0;
}
