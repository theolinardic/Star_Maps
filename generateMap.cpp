#include <generateMap.h>

// Start game window with GLFW and OpenGL and return the window reference.
GLFWwindow* initialize_glfw_window(void)
{
	// TO-DO: Implement function to load settings.json and pull the correct
	// values for res_width, res_height, etc...
	int res_width = 1920;
	int res_height = 1080;
	bool should_fullscreen = false;
	int graphics_preset = 0;

	// Set up correct version of OpenGL and select the core profile.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

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

	// Use glad to load OpenGl and set it up with the same settings as the glfw window.
	gladLoadGL();
	glViewport(0, 0, res_width, res_height);

	// Enable UI Debug tools on main game window.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(star_maps_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	return star_maps_window;
}

// Generate cubemap with the images supplied in faces[].
GLuint load_cubemap(const char* faces[])
{
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

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

	// Texture filter will perform interpolation on pixel color if the cube map is displayed larger/smaller than the textures resolution.
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Seamless cube map should make each corner of the cubemap merge correctly.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Return reference to generated cubemap.
	return texture_id;
}

skybox::skybox()
{
	this->shader = load_shader("shaders/skybox/vert.glsl", "shaders/skybox/frag.glsl");
	glUseProgram(this->shader);

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
	const char* faces[] = {
		"assets/images/skybox/back.png",
		"assets/images/skybox/middle.png",
		"assets/images/skybox/bottom.png",
		"assets/images/skybox/top.png",
		"assets/images/skybox/right.png",
		"assets/images/skybox/left.png"
	};

	this->textureID = load_cubemap(faces);

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

void skybox::render(glm::vec3 camera_position, glm::vec3 camera_front)
{
	glUseProgram(this->shader);

	glBindVertexArray(this->vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

	glUniform1i(glGetUniformLocation(this->shader, "skybox"), 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 4000.0f);
	glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(this->shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

}
