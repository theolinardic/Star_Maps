#include "hud_control.h"

// Class intialization function for a HUD element object.
element::element(int id)
{
    this->ui_id = id;
    this->should_render = true;
    this->status = 0;
    this->deleted = false;

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
        img_file = "assets/textures/ui/tile_selector/tiles_none_selected.png";
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
    case 5: // System Launcher Tile Preview
        img_file = "assets/textures/ui/tile_previews/sl_preview.png";
        break;
    case 6: // Galaxy Launcher Tile Preview
        img_file = "assets/textures/ui/tile_previews/gl_preview.png";
        break;
    case 7: // Housing Tile Preview
        img_file = "assets/textures/ui/tile_previews/hs_preview.png";
        break;
    case 8: // Mass Housing Tile Preview
        img_file = "assets/textures/ui/tile_previews/mhs_preview.png";
        break;
    case 9: // Power Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/ps_preview.png";
        break;
    case 10: // Ship Factory Tile Preview
        img_file = "assets/textures/ui/tile_previews/sf_preview.png";
        break;
    case 11: // Ship Store Tile Preview
        img_file = "assets/textures/ui/tile_previews/ss_preview.png";
        break;
    case 12: // Food Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/fs_preview.png";
        break;
    case 13: // Weapons Store Tile Preview
        img_file = "assets/textures/ui/tile_previews/ws_preview.png";
        break;
    case 14: // Bank Station Tile Preview
        img_file = "assets/textures/ui/tile_previews/bs_preview.png";
        break;
    case 15: // Holo Drive In Preview
        img_file = "assets/textures/ui/tile_previews/hdi_preview.png";
        break;
    case 16: // Amusement Park Tile Preview
        img_file = "assets/textures/ui/tile_previews/ap_preview.png";
        break;
    case 17: // Menu Logo
        img_file = "assets/images/logos/final_logo.png";
        break;
    case 18: // Menu new/load save buttons
        img_file = "assets/textures/ui/menu_buttons/menu_default.png";
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
    case 17: // Menu Logo
        adjustmentX = 0.0f;
        adjustmentY = -0.45f;
        size = 0.5f;
        break;
    case 18: // Menu new/load save
        adjustmentX = 0.0f;
        adjustmentY = 0.5f;
        size = 0.2f;
        break;
    default: // Preview tiles
        adjustmentX = 1.09f;
        adjustmentY = 0.44f;
        size = 0.25f;
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

element::~element() {
    glDeleteTextures(1, &this->texture_id);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
    glDeleteVertexArrays(1, &this->VAO);
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
            new_img_file = "assets/textures/ui/tile_selector/tiles_none_selected.png";
            break;
        case 5: // System Launcher Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_sl_selected.png";
            break;
        case 6: // Galaxy Launcher Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_gl_selected.png";
            break;
        case 7: // Housing Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_hs_selected.png";
            break;
        case 8: // Mass Housing Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_mhs_selected.png";
            break;
        case 9: // Power Station Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_ps_selected.png";
            break;
        case 10: // Ship Factory Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_sf_selected.png";
            break;
        case 11: // Ship Store Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_ss_selected.png";
            break;
        case 12: // Food Station Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_fs_selected.png";
            break;
        case 13: // Weapons Store Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_ws_selected.png";
            break;
        case 14: // Bank Station Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_bs_selected.png";
            break;
        case 15: // Holo Drive In Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_hdi_selected.png";
            break;
        case 16: // Amusement Park Tile Preview
            new_img_file = "assets/textures/ui/tile_selector/tiles_ap_selected.png";
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
    case 18: // Save Menu button
        switch (new_status)
        {
        case 0: // default
            new_img_file = "assets/textures/ui/menu_buttons/menu_default.png";
            break;
        case 1: // hovering over new game
            new_img_file = "assets/textures/ui/menu_buttons/menu_new.png";
            break;
        case 2: // hovering over save game
            new_img_file = "assets/textures/ui/menu_buttons/menu_load.png";
            break;
        }
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
void element::render(glm::vec3 camera_position, glm::vec3 camera_front, GLFWwindow* window)
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
    if (id == 0) // Time & Day Bar Text
    {
        this->position = glm::vec2(750.0f, 1055.0f);
        this->scale = 0.50f;
    }
    if (id == 1) // Money Counter
    {
        this->position = glm::vec2(1568.0f, 22.0f);
        this->scale = 0.50f;
    }
    if (id == 2) // Money Label
    {
        this->position = glm::vec2(1570.0f, 46.0f);
        this->scale = 0.20f;
    }
        
    this->text_id = id;
    this->text = text;
    this->should_render = true;
    this->shader = load_shader("shaders/text/vert.glsl", "shaders/text/frag.glsl");
    glUseProgram(this->shader);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* monitor_settings = glfwGetVideoMode(monitor);
    float w = monitor_settings->width;
    float h = monitor_settings->height;
    glm::mat4 projection = glm::ortho(0.0f, w, 0.0f, h);

    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft) || FT_New_Face(ft, "assets/other/font_high_speed.ttf", 0, &face)) {
        std::cout << "Failed to load font: font_high_speed.ttf" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            std::cout << "failed to load Glyph" << std::endl;


        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        this->chars.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Setup buffers for text rendering (VAO, VBO):
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void text_element::update_text(std::string text)
{
    this->text = text;
}

void text_element::render()
{
    GLint depth_func;
    glGetIntegerv(GL_DEPTH_FUNC, &depth_func);

    // Temporarily disable depth testing so text renders on top
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glUseProgram(this->shader);
    glUniform3f(glGetUniformLocation(this->shader, "textColor"), 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    glm::vec2 start_pos = this->position;

    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = this->chars[*c];
        float xpos = this->position.x + ch.bearing.x * scale;
        float ypos = this->position.y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        this->position.x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    this->position = start_pos;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(depth_func);
}

// Class intialization function for a HUD object.
HUD::HUD(GLFWwindow* window)
{
    this->window = window;
    this->current_preview = -1;
    this->start_menu();
}

void HUD::start_menu()
{
    this->in_menu = true;
    this->add_element(17);
    this->add_element(18);
}

void HUD::start_game()
{
    this->in_menu = false;
    for (element* elm : this->all_elements)
        elm->should_render = false;
    this->add_element(0);
    this->add_element(1);
    this->add_element(2);
    this->add_element(3);

    this->add_text_element(0, "Day 0        12:00:00");
    this->add_text_element(1, "500,000,000");
    this->add_text_element(2, "Available Credits");
}

// Function to add new UI element.
void HUD::add_element(int element_id)
{
    if (element_id > 4 && element_id < 17)
    {
        if (current_preview == -1)
        {
            element* new_element = new element(element_id);
            this->all_elements.push_back(new_element);
            current_preview = element_id;
        }
    }
    else
    {
        element* new_element = new element(element_id);
        this->all_elements.push_back(new_element);
    }

}

// Function to add new text UI element.
void HUD::add_text_element(int element_id, std::string default_text)
{
    text_element* new_element = new text_element(element_id, default_text);
    this->all_text_elements.push_back(new_element);
}

void HUD::delete_preview()
{
    if (current_preview != -1)
    {
        for (element* elm : this->all_elements)
            if (elm->ui_id == current_preview)
            {
                elm->deleted = true;
                elm->should_render = false;
                //delete elm;
            }
                
        current_preview = -1;
    }
}

// Function to update image of a ui element.
void HUD::update_element(int element_id, int new_status)
{
    for (element* elm : this->all_elements)
        if (elm->ui_id == element_id)
            elm->switch_img(new_status);
}

// Function to update text of a ui element.
void HUD::update_text_element(int element_id, std::string new_text)
{
    for (text_element* elm : this->all_text_elements)
        if (elm->text_id == element_id)
            elm->update_text(new_text);
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
            elm->render(camera_position, camera_front, this->window);
    for (text_element* elm : this->all_text_elements)
        if (elm->should_render)
            elm->render();
}