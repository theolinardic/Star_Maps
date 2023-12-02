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
// Below are original source files for Star Maps:
#include <objects.h>
#include <generate_map.h>
#include <player.h>
#include <game_logic.h>
#include <hud_control.h>

int main()
{
	// Initialize GLFW window:
	GLFWwindow* star_maps_window = initialize_glfw_window();
	glfwSwapBuffers(star_maps_window);
	glEnable(GL_DEPTH_TEST);

	// These lines allow textures to keep their transparent backgrounds (useful for HUD):
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create skybox:
	skybox SB = skybox();

	// Set up camera:
	glm::vec3 camera_position = glm::vec3(642.0f, 692.0f, -1220.0f);
	glm::vec3 camera_front = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	float camera_yaw = 115.0f;
	float camera_pitch = -35.0f;

	// Start ImGUI Dev Tools and start recording the time the app has been open and the game unpaused:
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	float launch_time = glfwGetTime();
	float last_frame_time = glfwGetTime();
	float awake_time = 0;

	// Start the brain of the game:
	star_maps_game star_maps = star_maps_game(false);

	HUD* game_ui = new HUD();
	int screenWidth, screenHeight;
	double fps;
	bool fullscreen = true;

	// Main game/render loop:
	while (!glfwWindowShouldClose(star_maps_window))
	{
		// Clear OpenGL buffers:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the timer for how long game has been unpaused:
		if (!star_maps.paused)
			awake_time += (glfwGetTime() - last_frame_time);

		double currentTime = glfwGetTime();
		static int frameCount = 0;
		static double lastTime = 0.0;
		frameCount++;
		if (currentTime - lastTime >= 1.0) {
			fps = static_cast<double>(frameCount) / (currentTime - lastTime);
			frameCount = 0;
			lastTime = currentTime;
		}

		glfwGetFramebufferSize(star_maps_window, &screenWidth, &screenHeight);

		std::string window_head = "Star Maps v0.35: " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight)
			+ " - FPS: " + std::to_string(static_cast<int>(fps));
		glfwSetWindowTitle(star_maps_window, window_head.c_str());

		// Tells GLFW to check for new events (such as mouse/keyboard inputs) and process them:
		glfwPollEvents();

		// Render new frame of debug tools at the start of every frame:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Give core game logic the time passed since the last frame for its debug tools:
		star_maps.update_debug_ingame_clock(glfwGetTime() - last_frame_time);
		last_frame_time = glfwGetTime();


		glm::vec3 front;
		front.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		front.y = sin(glm::radians(camera_pitch));
		front.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		camera_front = glm::normalize(front);

		// Don't read GLFW window input if mouse is interacting with debug tools, otherwise handle all player inputs in player.cpp:
		if (!io.WantCaptureMouse)
			read_player_input(star_maps_window, camera_position, camera_front, camera_yaw, camera_pitch, star_maps.entitiys);

		// Render the skybox and call the entity manager from the game_logic which handles rendering of all spawned game objects:
		SB.render(camera_position, camera_front);
		game_ui->render(camera_position, camera_front);
		star_maps.entity_manager(camera_position, camera_front, star_maps.game_speed_multiplier);

		// Create elements for the debug tools and render them to the screen:
		float time_open_sec = glfwGetTime() - launch_time;
		int time_open_min = time_open_sec / 60;
		ImGui::Begin("Debug Window");
		ImGui::Text("Window: %dx%d %.2f FPS", screenWidth, screenHeight, static_cast<float>(fps));
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
		if (ImGui::Button("Switch Camera View - Top"))
			star_maps.switch_camera_mode(camera_position, camera_front, camera_yaw, camera_pitch, 1);
		if (ImGui::Button("Switch Camera View - Side"))
			star_maps.switch_camera_mode(camera_position, camera_front, camera_yaw, camera_pitch, 0);
		if (ImGui::Button("Fullscreen"))
			switch_display_type(star_maps_window, true);
		if (ImGui::Button("Windowed"))
			switch_display_type(star_maps_window, false);
		ImGui::End();
		ImGui::Render();
		if (show_debug)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(star_maps_window);
	}

	// Clean up ImGui debug tools and GLFW window at the end of the program:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(star_maps_window);
	glfwTerminate();

	return 0;
}
