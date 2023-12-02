#include "hud_control.h"

// Class intialization function for a HUD object.
HUD::HUD()
{
    // Set up shader for the hud:
	this->shader = load_shader("shaders/ui_elements/vert.glsl", "shaders/ui_elements/frag.glsl");

    glGenTextures(1, &this->tiles_texture_id);
    glBindTexture(GL_TEXTURE_2D, this->tiles_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height;
    unsigned char* image = SOIL_load_image("assets/textures/ui/main_game/tiles_empty.png", &width, &height, 0, SOIL_LOAD_RGBA);
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    SOIL_free_image_data(image);

    GLfloat aspect_ratio = width / height;
    GLfloat size = 0.2f;

    GLfloat vertices[] = {
        -aspect_ratio * size, -size, 0.0f, 0.0f, 1.0f,
        -aspect_ratio * size, size, 0.0f, 0.0f, 0.0f,
        aspect_ratio * size, size, 0.0f, 1.0f, 0.0f,
        aspect_ratio * size, -size, 0.0f, 1.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Function to render the hud.
void HUD::render(glm::vec3 camera_position, glm::vec3 camera_front) {
    glUseProgram(this->shader);
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->tiles_texture_id);
    float aspectRatio = 1920.0f / 1080.0f;
    glm::mat4 projection_matrix = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

    glUniform1i(glGetUniformLocation(this->shader, "text"), 0);
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}