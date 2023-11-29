#pragma once

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>
// Below  are original source files for Star Maps:
#include <objects.h>
#include <generate_map.h>
#include <player.h>
#include <game_logic.h>

GLfloat lineVertices[] = {
	// Two points for the line (change as per your needs)
	0.0f, 0.0f,
	0.5f, 0.5f
};

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

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	float launch_time = glfwGetTime();
	float last_frame_time = glfwGetTime();
	float awake_time = 0;

	// Start the brain of the game with the core logic.
	star_maps_game star_maps = star_maps_game(false);

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

		// Render new frame of debug tools at the start of every opengl/glfw frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Aadd to the in game counter to increase sol/clock via debug menu in game logic class.
		star_maps.update_debug_ingame_clock(glfwGetTime() - last_frame_time);
		last_frame_time = glfwGetTime();
		// Don't read game input if mouse is interacting with debug dools.
		glm::vec3 front;
		front.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		front.y = sin(glm::radians(camera_pitch));
		front.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		camera_front = glm::normalize(front);

		SB.render(camera_position, camera_front);
		star_maps.entity_manager(camera_position, camera_front, star_maps.game_speed_multiplier);
		
		float time_open_sec = glfwGetTime() - launch_time;
		int time_open_min = time_open_sec / 60;

		if (!io.WantCaptureMouse)
		{
			glm::vec3 check_item = read_player_input(star_maps_window, camera_position, camera_front, camera_yaw, camera_pitch, star_maps.entitiys);
		}

		// Create elements for the debug tools to displa and render them to the screen.
		ImGui::Begin("Debug Window");
		if (time_open_min >= 1)
			ImGui::Text("Time open: %dm %.1fs", time_open_min, (time_open_sec - (60 * time_open_min)));
		else
			ImGui::Text("Time open: %.2fs", time_open_sec);
		ImGui::Text("Camera X: %f", camera_position.x);
		ImGui::Text("Camera Y: %f", camera_position.y);
		ImGui::Text("Camera Z: %f", camera_position.z);
		ImGui::Text("Camera Rot X: %f", front.x);
		ImGui::Text("Camera Rot Y: %f", front.y);
		ImGui::Text("Camera Rot Y: %f", front.z);
		ImGui::Checkbox("Paused", &star_maps.paused);
		if (ImGui::Button("Camera Top"))
		{
			star_maps.switch_camera_mode(camera_position, camera_front, camera_yaw, camera_pitch, 1);
		}
		if (ImGui::Button("Camera Side"))
		{
			star_maps.switch_camera_mode(camera_position, camera_front, camera_yaw, camera_pitch, 0);
		}
		ImGui::End();
		ImGui::Render();
		if (show_debug)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (loaded == false)
		{
			loaded = true;
		//	star_maps.load_save(3);
		}

		glfwSwapBuffers(star_maps_window);
	}

	// ImGui Dev UI and GLFW Clean Up at the end of program.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(star_maps_window);
	glfwTerminate();

	return 0;
}
