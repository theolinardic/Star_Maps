#include <objects.h>

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
    this->render_bb;
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

    boundingBoxMin = glm::vec3(std::numeric_limits<float>::max());
    boundingBoxMax = glm::vec3(-std::numeric_limits<float>::max());

    for (unsigned int i = 0; i < verticesCopy.size(); i += 3) {
        glm::vec3 vertex(verticesCopy[i], verticesCopy[i + 1], verticesCopy[i + 2]);
        boundingBoxMin = glm::min(boundingBoxMin, vertex);
        boundingBoxMax = glm::max(boundingBoxMax, vertex);
    }

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

glm::vec3 game_object::get_center() {
    glm::vec3 center(0.0f);
    unsigned int vertexCount = this->obj_verts.size() / 3; // Number of vertices, assuming 3 values per vertex

    for (unsigned int i = 0; i < this->obj_verts.size(); i += 3) {
        center.x += this->obj_verts[i];
        center.y += this->obj_verts[i + 1];
        center.z += this->obj_verts[i + 2];
    }

    center.x /= static_cast<float>(vertexCount); // Divide by the number of vertices
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

        this->position.x = orbitX;
        this->position.y = orbitY;
        this->position.z = orbitZ;

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

        this->position.x = orbitX;
        this->position.y = orbitY;
        this->position.z = orbitZ;

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

    this->vm = viewMatrix;
    this->pm = projectionMatrix;
    this->mm = modelMatrix;

}


void game_object::delete_object()
{
    free(this);
}

void game_object::RenderBoundingBox() {
    // Generate the bounding box vertices for a wireframe box
    std::vector<GLfloat> boundingBoxVertices = {
        // Bottom face
        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z,
        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z,

        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z,
        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z,

        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z,
        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z,

        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z,
        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z,

        // Top face
        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z,
        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z,

        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z,
        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z,

        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z,
        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z,

        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z,
        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z,

        // Side edges
        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z,
        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z,

        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z,
        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z,

        boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z,
        boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z,

        boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z,
        boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z,
    };

    // Generate VAO and VBO for the bounding box
    GLuint bboxVAO, bboxVBO;
    glGenVertexArrays(1, &bboxVAO);
    glBindVertexArray(bboxVAO);

    glGenBuffers(1, &bboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bboxVBO);
    glBufferData(GL_ARRAY_BUFFER, boundingBoxVertices.size() * sizeof(GLfloat), &boundingBoxVertices[0], GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Render the bounding box
    glUseProgram(this->shader);  // Use the correct shader program

    glBindVertexArray(bboxVAO);
    glDrawArrays(GL_LINES, 0, boundingBoxVertices.size() / 3);

    // Clean up
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &bboxVAO);
    glDeleteBuffers(1, &bboxVBO);
}



rings::rings() {
    // Load shader, create VAO, VBO, and set up vertices for a single ring
    this->shader = load_shader("shaders/orbit_rings/vert.glsl", "shaders/orbit_rings/frag.glsl");
    glUseProgram(this->shader);

    // Generate VAO and VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    // Bind VAO and VBO
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    // Vertices for a single ring
    std::vector<float> vertices;
    for (int j = 0; j < 360; j++) {
        float degInRad = j * PI / 180;
        vertices.push_back(cos(degInRad));
        vertices.push_back(sin(degInRad));
        vertices.push_back(0.0f); // Z-coordinate is 0 for now
    }

    // Fill VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void rings::render()
{
    glBindVertexArray(this->VAO);
    glLineWidth(1.0f);

    float radiuses[7] = { 400.0 , 645.0, 900.0, 1140.0, 1485.0, 1850.0 };

    // Incremental scaling for each ring
    float scaleFactor = 1.0f;
    for (int i = 0; i < 6; i++)
    {
        glm::mat4 modelorb = glm::mat4(1);
        modelorb = glm::translate(modelorb, glm::vec3(0.0f, 0.0f, 0.0f));
        // Increase the scaling factor for each ring

        modelorb = glm::rotate(modelorb, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));


        modelorb = glm::scale(modelorb, glm::vec3(radiuses[i] * 0.5f, radiuses[i] * 0.5f, radiuses[i] * 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, glm::value_ptr(modelorb));

        glDrawArrays(GL_LINE_LOOP, 0, 360);
    }
}

arrow::arrow() {
    // Load shader, create VAO, VBO, and set up vertices for a single ring
    this->shader = load_shader("shaders/orbit_rings/vert.glsl", "shaders/orbit_rings/frag.glsl");
    glUseProgram(this->shader);

    // Generate VAO and VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    // Bind VAO and VBO
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    GLfloat rayVertices[] = {
        641.959,  691.955, -1219.92,
        -3486.45, -3799.48, 6836.2
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(rayVertices), rayVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void arrow::render()
{
    glBindVertexArray(this->VAO);
    glLineWidth(1.0f);

    // Update the arrow's position and orientation in the 3D space
    glm::mat4 modelArrow = glm::mat4(1.0f);
    // Translate the arrow to a specific position in the scene
    modelArrow = glm::translate(modelArrow, glm::vec3(0.0f, 0.0f, 0.0f));
    // Apply a rotation if needed
    modelArrow = glm::rotate(modelArrow, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // Scale the arrow if necessary
    modelArrow = glm::scale(modelArrow, glm::vec3(1.0f, 1.0f, 1.0f));

    // Set the model matrix in the shader
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, glm::value_ptr(modelArrow));

    // Draw the arrow as a line from (0, 0, 0) to (100, 100, 100) in the transformed space
    glDrawArrays(GL_LINES, 0, 2);
}