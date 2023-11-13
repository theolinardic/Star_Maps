#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Below includes are original source files for Star Maps:
#include <test_glsl.h>
#include <generateMap.h>
#include <loadShader.h>
#include <loadObject.h>
#include <loadFiles.h>

int main()
{
	// Initialize GLFW window using function from generateMap.cpp.
	GLFWwindow* star_maps_window = initialize_glfw_window();

	glfwSwapBuffers(star_maps_window);

	// Load shaders:
	const char* vertex_shader_path = "shaders/default/vert.glsl";
	const char* fragment_shader_path = "shaders/default/frag.glsl";
	GLuint default_shader = load_shader(vertex_shader_path, fragment_shader_path);

	// Load objects:

	// Load textures:

	// Main game/render loop:
	while (!glfwWindowShouldClose(star_maps_window))
	{
		// Tells GLFW to check for new events and process them.
		glfwPollEvents();
	}

	// GLFW clean up at the end of the program.
	glfwDestroyWindow(star_maps_window);
	glfwTerminate();

	return 0;
}
