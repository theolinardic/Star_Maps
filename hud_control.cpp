#include "hud_control.h"

// Class intialization function for a HUD object.
HUD::HUD()
{
    // Set up shader for the hud:
	this->shader = load_shader("shaders/orbit_rings/vert.glsl", "shaders/orbit_rings/vert.glsl");
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);

	// Load initial default hud elements:
	glGenTextures(1, &this->tiles_texture_id);
	std::ifstream infile("assets/textures/ui/main_game/tiles_empty.png");
	glBindTexture(GL_TEXTURE_2D, this->tiles_texture_id);
	if (infile.good() != 1)
		std::cout << "Err: " << infile.good() << std::endl;
	int w, h;
	unsigned char* texture_img = SOIL_load_image("assets/textures/ui/main_game/tiles_empty.png", &w, &h, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_img);
	SOIL_free_image_data(texture_img);
}

// Function to render the hud.
void HUD::render()
{
    // Load the hud shader and all the individual UI pieces textures:
    glUseProgram(this->shader);
    glBindTexture(GL_TEXTURE_2D, this->tiles_texture_id);
    glBindVertexArray(this->VAO);

    int textureWidth, textureHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight);

    // Define vertices and indices for the textures to be displayed on:
    float vertices[] = {
        0.0f, static_cast<float>(textureHeight), 0.0f, 1.0f,
        static_cast<float>(textureWidth), static_cast<float>(textureHeight), 1.0f, 1.0f,
        static_cast<float>(textureWidth), 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2, 
        0, 2, 3 
    };

    // Bind the VBO and EBO and pass the vertices and indices in:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex attributes pointers:
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Draw the hud to the screen:
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
