#pragma once

#include <vector>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <SOIL2/SOIL2.h>
#include <json.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class game_object {
public:
    int planet_entity_id, indices_count, parent, orbit_location;
    bool render_bb, is_preview;
    float rotation_speed, size_adjust, orbit_radius, orbit_speed, time_exist, last_frame_time;
    glm::vec3 position, orbit_center, boundingBoxMin, boundingBoxMax;
    glm::mat4 model_matrix, parent_mm, view_matrix, projection_matrix;
    game_object* parent_planet;
    std::vector<GLfloat> obj_verts;
    std::string name;
    GLuint VAO, VBO, EBO, textureID, shader;
    GLFWwindow* window;

    bool is_npc;
    float time_to_fly;
    glm::vec3 start_pos = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 end_pos;
    int start_par = -1;
    int end_par = -1;

    game_object(GLFWwindow* window, bool is_npc);
    void load_object(const char* obj_file);
    void load_texture(const char* texture_file);
    glm::vec3 get_center();
    float get_radius();
    void get_rand_pars(std::vector<game_object*> entitiys);
    void render(const glm::vec3& camera_position, const glm::vec3& camera_front, float game_speed, std::vector<glm::vec3> ring_positions[7], std::vector<game_object*> entitiys);
    void render_bounding_box();
};

class rings
{
public:
    GLuint shader, VAO, VBO;
    std::vector<glm::vec3> ring_positions[7];
    rings();
    void render(const glm::vec3& camera_position, const glm::vec3& camera_front);
};

GLchar* read_shader_text(const char* file_path);
GLuint load_shader(const char* vertex_file_path, const char* frag_file_path);