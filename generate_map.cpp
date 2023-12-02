#include <generate_map.h>

// Function to start game window with GLFW and OpenGL and return the window reference.
GLFWwindow* initialize_glfw_window(void)
{
	// TO-DO: Implement function to load settings.json and pull the correct
	// values for res_width, res_height, etc...
	int res_width = 1920;
	int res_height = 1080;
	bool should_fullscreen = false;
	int graphics_preset = 0;

	// Set up correct version of OpenGL and select the core profile:
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

	// Get the video mode of the primary monitor
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	// Generate GLFW window:
	// TO-DO: Update first null value to what should_fullscreen is.
	GLFWwindow* star_maps_window = glfwCreateWindow(mode->width, mode->height, "Star Maps", primaryMonitor, nullptr);

	// Check to make sure window successfully opened:
	if (star_maps_window == NULL)
	{
		std::cout << "Failed to create glfw window. Closing Star Maps." << std::endl;
		glfwTerminate();
		return NULL;

	}
	// Make 'star_maps_window' the active window in glfw:
	glfwMakeContextCurrent(star_maps_window);

	// Use glad to load OpenGL and set it up with the same settings as the glfw window:
	gladLoadGL();
	glViewport(0, 0, res_width, res_height);

	// Enable ImGui Debug tools on main game window:
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(star_maps_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Return reference to the glfw window:
	return star_maps_window;
}

// Function to generate a cubemap with the images supplied in faces[] for the skybox.
GLuint load_cubemap(const char* faces[])
{
	// Generate texture for a cube map texture:
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	// Use SOIL2 to load the images and place them onto a cubemap:
	int width, height, channels;
	for (GLuint i = 0; i < 6; ++i)
	{
		unsigned char* data = SOIL_load_image(faces[i], &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else {
			std::cout << "Error in opening cubemap texture: " << faces[i] << "." << std::endl;
			SOIL_free_image_data(data);
			return 0;
		}
	}

	// Set up basic parameters for the cube map to help visuals:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Return the texture id of the generated cubemap:
	return texture_id;
}

// Initialization function for a skybox class object.
skybox::skybox()
{
	// Load a skybox shader:
	this->shader = load_shader("shaders/skybox/vert.glsl", "shaders/skybox/frag.glsl");
	glUseProgram(this->shader);

	// Define the vertices/indices for the skybox:
	GLfloat vertices[] =
	{
		-1500.0f, -1500.0f, 1500.0f,
		1500.0f, -1500.0f, 1500.0f,
		1500.0f, -1500.0f, -1500.0f,
		-1500.0f, -1500.0f, -1500.0f,
		-1500.0f, 1500.0f, 1500.0f,
		1500.0f, 1500.0f, 1500.0f,
		1500.0f, 1500.0f, -1500.0f,
		-1500.0f, 1500.0f, -1500.0f,
	};
	GLuint indices[] =
	{
		1,2,6,
		6,5,1,
		0,4,7,
		7,3,0,
		4,5,6,
		6,7,4,
		0,3,2,
		2,1,0,
		0,1,5,
		5,4,0,
		3,7,6,
		6,2,3
	};
	// Define the locations of all six skybox cube map textures:
	const char* faces[] = {
		"assets/images/skybox/back.png",
		"assets/images/skybox/middle.png",
		"assets/images/skybox/bottom.png",
		"assets/images/skybox/top.png",
		"assets/images/skybox/right.png",
		"assets/images/skybox/left.png"
	};

	// Load the cube map and save it to the skybox objects textureID:
	this->texture_id = load_cubemap(faces);

	// Create the skybox VAO, VBO, and EBO and fill them with the previously defined vertices and indices:
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &this->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

// Function to render the skybox.
void skybox::render(glm::vec3 camera_position, glm::vec3 camera_front)
{
	// Fairly straightfoward... load the shader and texture from the skybox object and draw it to the screen:
	glUseProgram(this->shader);

	glBindVertexArray(this->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_id);

	glUniform1i(glGetUniformLocation(this->shader, "skybox"), 0);

	// Define view and projection matrices of the skybox from the camera FOV, position, front, and the windows aspect ratio:
	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 4000.0f);
	glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));

	// Pass view and projection matrices to the skybox shader:
	glUniformMatrix4fv(glGetUniformLocation(this->shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

	// Draw the skybox:
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
