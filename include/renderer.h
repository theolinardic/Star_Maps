#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <SOIL2/SOIL2.h>
#include <loadShader.h>
#include <json.hpp>
#include <random>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

class game_object {
private:
    int indicesCount;

public:
    float addx = 1, addy = 0, addz = 1;
    float addxR = 0, addyR = 1, addzR = 1;
    game_object();
    void new_planet();
    int planet_entity_id;
    glm::vec3 position;
    float rotation_speed;
    glm::vec3 orbit_center;
    float size_adjust;
    float orbit_radius;
    float orbit_speed;
    int parent;
    game_object* parent_planet;
    std::vector<GLfloat> obj_verts;
    float time_exist;
    float last_frame_time;
    int orbit_location, texture_num;
    std::string name;
    GLuint VAO, VBO, EBO, textureID, shader, NormalBuffer, VBO_Vertices, VBO_UVs;
    std::vector<GLfloat> obj_uvs;
    glm::mat4 model_matrix;
    void LoadObject(const char* objFilePath);
    void LoadTexture(const char* textureFilePath);
    void Render(const glm::vec3& cameraPosition, const glm::vec3& cameraFront, float game_speed);
    void delete_object();
    void teleport(glm::vec3 new_pos);
    glm::vec3 get_center();
    glm::mat4 parent_mm;
    bool IsMouseOnObject(const glm::vec3& mouseRayOrigin, const glm::vec3& mouseRayDir, const glm::vec3& objectCenter, float objectRadius);
    float get_radius();
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;

    Vertex(glm::vec3 pos, glm::vec2 texCoord = glm::vec2(0, 0), glm::vec3 normal = glm::vec3(0, 0, 0)) :
        pos(pos),
        texCoord(texCoord),
        normal(normal) {}
};