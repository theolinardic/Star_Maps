#include <test_glsl.h>

void maintest()
{
	// Initialize glfw and set up the profile for opengl.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the glfw window with res_width, res_height, name, fullscreen, and context sharing.
	// The if statement is to make sure that the window opened successfully.
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Star Maps", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	// Make 'window' the active window in glfw.
	glfwMakeContextCurrent(window);

	// Tell glad to load opengl.
	gladLoadGL();

	// Define the viewport in opengl. 0 0 is bottom left of window and 1280 720 is top right of window.
	glViewport(0, 0, 1280, 720);

	// Clear the color of the new frame buffer and set it to the new color.
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	// Rendering loop:
	while (!glfwWindowShouldClose(window))
	{
		// Tells glfw to check for new events and process them. 
		glfwPollEvents();
	}

	// Close the window and close glfw.
	glfwDestroyWindow(window);
	glfwTerminate();
}