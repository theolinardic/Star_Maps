#include <renderer.h>

game_object::game_object()
{
    this->VAO = 0;
    this->EBO = 0;
    this->VBO = 0;
    this->indicesCount = 0;
    this->textureID = 2;
    this->last_frame_time = glfwGetTime();
    this->time_exist = 0.0;
    this->rotation_speed = 0.5;
    this->position = glm::vec3(0.0, 0.0, 0.0);
}


void game_object::LoadObject(const char* objFilePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objFilePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }

    aiMesh* mesh = scene->mMeshes[0]; // Assuming there's only one mesh in the file

    std::vector<GLfloat> vertices; // Combined buffer for vertices (positions, UVs, normals)
    std::vector<GLuint> indices;   // Indices buffer
    std::vector<GLfloat> verticesCopy;

    // Process mesh data
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3D vertex = mesh->mVertices[i];
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
        verticesCopy.push_back(vertex.x);
        verticesCopy.push_back(vertex.y);
        verticesCopy.push_back(vertex.z);

        aiVector3D uv = mesh->mTextureCoords[0][i];
        vertices.push_back(uv.x);
        vertices.push_back(uv.y);

        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) {
            std::cout << "Out-of-range UV: (" << uv.x << ", " << uv.y << ")" << std::endl;
        }

        if (mesh->HasNormals()) {
            aiVector3D normal = mesh->mNormals[i];
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create and bind VAO
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // Vertices buffer (positions, UVs, normals)
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    // Indices buffer
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Position attribute (from combined vertices buffer)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

    // UVS attribute (from combined vertices buffer)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    // Normals attribute (from combined vertices buffer)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    this->indicesCount = indices.size();
    this->obj_verts = verticesCopy;
    // Debug print to check vertices and UVs alignment
    std::cout << "Number of Vertices: " << vertices.size() / 8 << std::endl;
    std::cout << "Number of Indices: " << indices.size() / 3 << std::endl;
}

void game_object::LoadTexture(const char* textureFilePath) {
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = SOIL_load_image(textureFilePath, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(data);
    }
    else {
        std::cout << "Failed to load texture: " << textureFilePath << std::endl;
    }
}

void game_object::teleport(glm::vec3 new_pos)
{
    this->position = new_pos;
}

glm::vec3 game_object::get_center()
{
    glm::vec3 center(0.0f);
    unsigned int vertexCount = this->obj_verts.size() / 3;

    for (unsigned int i = 0; i < this->obj_verts.size(); i += 3) {
        center.x += this->obj_verts[i];
        center.y += this->obj_verts[i + 1];
        center.z += this->obj_verts[i + 2];
       // std::cout << "Sum: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
    }
    center.x /= static_cast<float>(vertexCount);
    center.y /= static_cast<float>(vertexCount);
    center.z /= static_cast<float>(vertexCount);
    return center;
}

float game_object::get_radius() {
    glm::vec3 center = get_center();
    float maxDistance = 0.0f;

    for (unsigned int i = 0; i < this->obj_verts.size(); i += 3) {
        float distance = glm::distance(center, glm::vec3(this->obj_verts[i], this->obj_verts[i + 1], this->obj_verts[i + 2]));
        if (distance > maxDistance) {
            maxDistance = distance;
        }
    }

    return maxDistance;
}


void game_object::Render(const glm::vec3& cameraPosition, const glm::vec3& cameraFront, float game_speed)
{
    // Initialize debug menu reference:
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Text("%d %f %f %f", this->planet_entity_id, this->position.x, this->position.y, this->position.z);

    // Update the time the object has existing so that rotation can be adjusted accordingly and rotate the object around the Y and Z axis.
    this->time_exist += (glfwGetTime() - last_frame_time) * game_speed;
    this->last_frame_time = glfwGetTime();
    
    // Bind correct VAO and shader for the current object:
    glBindVertexArray(this->VAO);
    glUseProgram(this->shader);

    // Initialize an the model matrix of changes to be made to the object and start by adding the size adjustment.
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(this->size_adjust));
    glm::vec3 orbitCenter;

    if (this->planet_entity_id > 1)
    {
        glm::vec3 parentPlanetCenter = this->parent_planet->get_center();
        glm::vec4 parentPlanetCenter4(parentPlanetCenter.x, parentPlanetCenter.y, parentPlanetCenter.z, 1.0f);

        float orbitX = parentPlanetCenter4.x + this->orbit_radius * cos(this->time_exist * this->orbit_speed);
        float orbitY = parentPlanetCenter4.y; // Assuming the orbit is on the same Y-level as the parent planet
        float orbitZ = parentPlanetCenter4.z + this->orbit_radius * sin(this->time_exist * this->orbit_speed);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(orbitX, orbitY, orbitZ));
        modelMatrix = this->parent_planet->parent_mm * modelMatrix;

        orbitCenter = glm::vec3(parentPlanetCenter4.x, parentPlanetCenter4.y, parentPlanetCenter4.z);

    }
    else
    {
        // Calculate position on the orbit path
        float orbitX = this->orbit_center.x + this->orbit_radius * cos(this->time_exist * this->orbit_speed);
        float orbitY = 0.0f;
        float orbitZ = this->orbit_center.z + this->orbit_radius * sin(this->time_exist * this->orbit_speed);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(orbitX, orbitY, orbitZ));

        orbitCenter = this->orbit_center;
    }

    float fov = 45.0f;
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 10000.0f);

    // Pass matrices to the shader
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniform1f(glGetUniformLocation(this->shader, "rotation_angle"), (glm::radians(this->time_exist)));
    //glUniformMatrix4fv(glGetUniformLocation(this->shader, "translate"), 1, GL_FALSE, glm::value_ptr(this->position));

   // this->position = abs(glm::vec3(orbitX, orbitY, orbitZ) - this->position);

    glBindTexture(GL_TEXTURE_2D, this->textureID);

    // Draw the object
    glDrawElements(GL_TRIANGLES, this->indicesCount, GL_UNSIGNED_INT, 0);

    this->parent_mm = modelMatrix;
}

bool game_object::IsMouseOnObject(const glm::vec3& mouseRayOrigin, const glm::vec3& mouseRayDir, const glm::vec3& objectCenter, float objectRadius) {
    // Calculate the vector from the mouse's ray origin to the object's center
    glm::vec3 mouseToCenter = objectCenter - mouseRayOrigin;

    // Project the mouse-to-center vector onto the mouse's ray direction to find the closest point on the ray to the sphere
    float projection = glm::dot(mouseToCenter, mouseRayDir);
    if (projection < 0) {
        // The object is behind the ray
        return false;
    }

    // Calculate the closest point on the ray to the sphere
    glm::vec3 closestPoint = mouseRayOrigin + projection * mouseRayDir;

    // Check if the closest point is within the sphere's radius
    float distance = glm::distance(closestPoint, objectCenter);
    return distance <= objectRadius;
}

void game_object::delete_object()
{
    free(this);
}