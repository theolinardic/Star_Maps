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

text_element::text_element(int id, std::string text)
{
    // update with switch
    if (id == 0)
        this->position = glm::vec2(50.0f, 50.0f);
    this->text = text;
    this->should_render = true;
    this->shader = load_shader("shaders/text/vert.glsl", "shaders/text/frag.glsl");
    glUseProgram(this->shader);

    // Generate texture for glyphs:
   // glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &this->texture_id);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    glUniform1i(glGetUniformLocation(this->shader, "text"), 0);

    // Setup buffers for text rendering (VAO, VBO):
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    // Define vertex attributes for text rendering:
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void text_element::render()
{
    glUseProgram(this->shader);
    glBindVertexArray(this->VAO);

    for (auto character : this->text) {
        // Load glyph for the current character:
        FT_Library ft;
        FT_Face face;
        if (FT_Init_FreeType(&ft) || FT_New_Face(ft, "assets/other/font_high_speed.ttf", 0, &face)) {
            std::cout << "Failed to load font: font_high_speed.ttf" << std::endl;
            return;
        }
        FT_Set_Pixel_Sizes(face, 0, 48);

        if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
            std::cout << "Failed to load glyph" << std::endl;
            continue;
        }

        // Generate texture for the current character:
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Update vertex buffer with the current character's position and size:
        float w = face->glyph->bitmap.width;
        float h = face->glyph->bitmap.rows;

        GLfloat vertices[6][4] = {
            { this->position.x,     this->position.y + h,   0.0, 0.0},
            { this->position.x,     this->position.y,       0.0, 1.0 },
            { this->position.x + w, this->position.y,       1.0, 1.0 },

            { this->position.x,     this->position.y + h,   0.0, 0.0 },
            { this->position.x + w, this->position.y,       1.0, 1.0 },
            { this->position.x + w, this->position.y + h,   1.0, 0.0 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursors for next glyph
        this->position.x += face->glyph->advance.x >> 6;

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    // Set text color to white:
    glUniform3f(glGetUniformLocation(this->shader, "textColor"), 1.0f, 1.0f, 1.0f);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1920), 0.0f, static_cast<float>(1080));
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(0);
}

// Class intialization function for a HUD object.
HUD::HUD()
{
    this->add_element(0);
    this->add_element(1);
    this->add_element(2);
    this->add_element(3);

    this->add_text_element(0, "Time: Test");
}

// Function to add new UI element.
void HUD::add_element(int element_id)
{
    element* new_element = new element(element_id);
    this->all_elements.push_back(new_element);
}

// Function to add new text UI element.
void HUD::add_text_element(int element_id, std::string default_text)
{
    text_element* new_element = new text_element(element_id, default_text);
    this->all_text_elements.push_back(new_element);
}

// Function to update image of a ui element.
void HUD::update_element(int element_id, int new_status)
{
    for (element* elm : this->all_elements)
        if (elm->ui_id == element_id)
            elm->switch_img(new_status);
}

// Function to hide all hud elements.
void HUD::hide_hud()
{
    for (element* elm : this->all_elements)
        elm->should_render = false;
    for (text_element* elm : this->all_text_elements)
        elm->should_render = false;
}

// Function to show all hud elements.
void HUD::show_hud()
{
    for (element* elm : this->all_elements)
        elm->should_render = true;
    for (text_element* elm : this->all_text_elements)
        elm->should_render = true;
}

// Function to render the hud.
void HUD::render(glm::vec3 camera_position, glm::vec3 camera_front)
{
    for (element* elm : this->all_elements)
        if (elm->should_render)
            elm->render(camera_position, camera_front);
    for (text_element* elm : this->all_text_elements)
        if (elm->should_render)
            elm->render();
}