#include <generateMap.h>

// Function to initialize the GLFW window and return it.
GLFWwindow* initialize_glfw_window(void)
{
	// TO-DO: Implement function to load settings.json and pull the correct
	// values for res_width, res_height, etc...
	int res_width = 1280;
	int res_height = 720;
	bool should_fullscreen = false;

	// Set up correct version of OpenGL and select the core profile.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Generate GLFW window.
	// TO-DO: Update first null value to what should_fullscreen is.
	GLFWwindow* star_maps_window = glfwCreateWindow(res_width, res_height, WINDOW_NAME, NULL, NULL);
	// Check to make sure window successfully opened.
	if (star_maps_window == NULL)
	{
		std::cout << "Failed to create glfw window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make 'star_maps_window' the active window in glfw.
	glfwMakeContextCurrent(star_maps_window);

	// Return the generated GLFW window.
	return star_maps_window;
}