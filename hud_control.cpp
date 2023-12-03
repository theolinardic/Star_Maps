#include "hud_control.h"

// Class intialization function for a HUD element object.
element::element(int id)
{
    this->ui_id = id;
    this->should_render = true;
    this->status = 0;

    // Set up shader for the hud:
    this->shader = load_shader("shaders/ui_elements/vert.glsl", "shaders/ui_elements/frag.glsl");

    glGenTextures(1, &this->texture_id);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::string img_file;

    switch (this->ui_id)
    {
    case 0: // Tile Selector
        img_file = "assets/textures/ui/tile_selector/tiles_empty.png";
        break;
    case 1: // Tool Selector
        img_file = "assets/textures/ui/tools_selector/tools_none_selected.png";
        break;
    case 2: // Progress Bar
        img_file = "assets/textures/ui/progress_bar/progress_bars_empty.png";
            break;
    case 3: // Time Bar
        img_file = "assets/textures/ui/time_bar/time_bar_empty.png";
            break;
    case 5: // System Travel Port Tile Preview
        img_file = "assets/textures/ui/tile_previews/stp_preview.png";
        break;
    case 6: // Galaxy Travel Port Tile Preview
        img_file = "assets/textures/ui/tile_previews/gtp_preview.png";
        break;
    case 7: // Housing Tile Preview
        img_file = "assets/textures/ui/tile_previews/housing_preview.png";
        break;
    case 8: // Mass Housing Tile Preview
        img_file = "assets/textures/ui/tile_previews/mh_preview.png";
        break;
    case 9: // Ship Factory Tile Preview
        img_file = "assets/textures/ui/tile_previews/sf_preview.png";
        break;
    case 10: // Power Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/ps_preview.png";
        break;
    case 11: // Ship Store Tile Preview
        img_file = "assets/textures/ui/tile_previews/ship_store_preview.png";
        break;
    case 12: // Food Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/fs_preview.png";
        break;
    case 13: // Weapons Store Tile Preview
        img_file = "assets/textures/ui/tile_previews/ws_preview.png";
        break;
    case 14: // Bank Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/bank_preview.png";
        break;
    case 15: // Amusement Park Tile Preview
        img_file = "assets/textures/ui/tile_previews/park_preview.png";
        break;
    default:
        std::cout << "Error. ui id type: " << this->ui_id << " is not valid." << std::endl;
    }

    unsigned char* image = SOIL_load_image(img_file.c_str(), &this->img_width, &this->img_height, 0, SOIL_LOAD_RGBA); // Note: c._str() casts std::string to cons* char
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture: " << img_file << std::endl;
    SOIL_free_image_data(image);

    this->aspect_ratio = static_cast<float>(this->img_width) / static_cast<float>(this->img_height);
    printf("%f ", this->aspect_ratio);
    GLfloat size = 0.155f;
    float adjustmentX, adjustmentY;
    
    switch (this->ui_id)
    {
    case 0: // Tile Selector
        adjustmentX = 1.08f;
        adjustmentY = 0.85f;
        break;
    case 1: // Tool Selector
        adjustmentX = -0.35f;
        adjustmentY = 0.85f;
        break;
    case 2: // Progress Bar
        adjustmentX = -1.4f;
        adjustmentY = 0.80f;
        size = 0.2f;
        break;
    case 3: // Time Bar
        adjustmentX = 0.0f;
        adjustmentY = -0.965f;
        size = 0.04f;
        break;
    }
    
    GLfloat vertices[] = {
        -this->aspect_ratio * size - adjustmentX, -size - adjustmentY, 0.0f, 0.0f, 1.0f, // bottom left
        -this->aspect_ratio * size - adjustmentX, size - adjustmentY, 0.0f, 0.0f, 0.0f, // top left
        this->aspect_ratio * size - adjustmentX, size - adjustmentY, 0.0f, 1.0f, 0.0f, // bottom right
        this->aspect_ratio * size - adjustmentX, -size - adjustmentY, 0.0f, 1.0f, 1.0f // top right
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

// Function to change the image of a ui element.
void element::switch_img(int new_status)
{
    // If image is the same as the current, no need to go through the process to swap the texture:
    if (this->status == new_status)
        return;
    this->status = new_status;

    std::string new_img_file;

    switch (this->ui_id)
    {
    case 0: // Tile Selector
        switch (new_status)
        {
        case 0: // Default/None Selected
            new_img_file = "assets/textures/ui/tile_selector/tiles_empty.png";
            break;
        case 1:
            break;
        default:
            std::cout << "Error status type: " << new_status << " on ui id: " << this->ui_id << "is not valid." << std::endl;
        }
        break;
    case 1: // Tool Selector
        switch (new_status)
        {
        case 0: // Default/None Selected
            new_img_file = "assets/textures/ui/tools_selector/tools_none_selected.png";
            break;
        case 1: // Pick/Move Selected
            new_img_file = "assets/textures/ui/tools_selector/tools_pick_selected.png";
            break;
        case 2: // Delete Selected
            new_img_file = "assets/textures/ui/tools_selector/tools_delete_selected.png";
            break;
        case 3: // Hovering over save
            new_img_file = "assets/textures/ui/tools_selector/tools_save_preview.png";
            break;
        case 4: // Hovering over quit
            new_img_file = "assets/textures/ui/tools_selector/tools_quit_preview.png";
            break;
        case 5: // Hovering over pick
            new_img_file = "assets/textures/ui/tools_selector/tools_pick_preview.png";
            break;
        case 6: // Hovering over delete
            new_img_file = "assets/textures/ui/tools_selector/tools_delete_preview.png";
            break;
        default:
            std::cout << "Error status type: " << new_status << " on ui id: " << this->ui_id << "is not valid." << std::endl;
        }
        break;
    case 2: // Progress Bar
        switch (new_status)
        {
        case 0: // Default/Empty
            new_img_file = "assets/textures/ui/progress_bar/progress_bars_empty.png";
            break;
        case 1:
            break;
        default:
            std::cout << "Error status type: " << new_status << " on ui id: " << this->ui_id << "is not valid." << std::endl;
        }
        break;
    case 3: // Time Bar
        switch (new_status)
        {
        case 0: // Default
            new_img_file = "assets/textures/ui/time_bar/time_bar_empty.png";
            break;
        case 1: // Paused
            new_img_file = "assets/textures/ui/time_bar/time_bar_pause.png";
            break;
        case 2: // Play
            new_img_file = "assets/textures/ui/time_bar/time_bar_play.png";
            break;
        case 3: // Fast Forward
            new_img_file = "assets/textures/ui/time_bar/time_bar_ffwd.png";
            break;
        default:
            std::cout << "Error status type: " << new_status << " on ui id: " << this->ui_id << "is not valid." << std::endl;
        }
        break;
    default:
        std::cout << "Error. ui id type: " << this->ui_id << " is not valid." << std::endl;
    }

    unsigned char* image = SOIL_load_image(new_img_file.c_str(), &this->img_width, &this->img_height, 0, SOIL_LOAD_RGBA);
    if (image) {
        glBindTexture(GL_TEXTURE_2D, this->texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Update aspect ratio if necessary
        this->aspect_ratio = static_cast<float>(this->img_width) / static_cast<float>(this->img_height);

        SOIL_free_image_data(image);
    }
    else
        std::cout << "Failed to load texture on swap: " << new_img_file << std::endl;
}

// Function to render invidual ui element.
void element::render(glm::vec3 camera_position, glm::vec3 camera_front)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* monitor_settings = glfwGetVideoMode(monitor);

    glUseProgram(this->shader);
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);

    float w = monitor_settings->width;
    float h = monitor_settings->height;
    glm::mat4 projection_matrix = glm::ortho(-(w/h), w/h, -1.0f, 1.0f, -1.0f, 1.0f);

    glUniform1i(glGetUniformLocation(this->shader, "text"), 0);
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// Class intialization function for a HUD object.
HUD::HUD()
{
    this->should_render = true;
    this->add_element(0);
    this->add_element(1);
    this->add_element(2);
    this->add_element(3);
}

// Function to add new UI element.
void HUD::add_element(int element_id)
{
    element* new_element = new element(element_id);
    this->all_elements.push_back(new_element);
}

// Function to update image of a ui element.
void HUD::update_element(int element_id, int new_status)
{
    for (element* elm : this->all_elements)
        if (elm->ui_id == element_id)
            elm->switch_img(new_status);
}

// Function to render the hud.
void HUD::render(glm::vec3 camera_position, glm::vec3 camera_front)
{
    for (element* elm : this->all_elements)
        if (elm->should_render)
            elm->render(camera_position, camera_front);
}