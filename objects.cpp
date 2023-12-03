#include <objects.h>

// Class initialization function for game objects.
game_object::game_object(GLFWwindow *window)
{
    this->VAO = 0;
    this->EBO = 0;
    this->VBO = 0;
    this->indices_count = 0;
    this->textureID = 0;
    this->last_frame_time = glfwGetTime();
    this->time_exist = 0.0;
    this->rotation_speed = 0.5;
    this->position = glm::vec3(0.0, 0.0, 0.0);
    this->render_bb = false;
    this->window = window;
}

// Function to load obj files for game objects using Assimp.
void game_object::load_object(const char* obj_file) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(obj_file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error loading obj: " << obj_file << ". Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }

    aiMesh* mesh = scene->mMeshes[0];
    std::vector<GLfloat> buffer; // Combined buffer for vertices, UVs, normals
    std::vector<GLuint> indices;   // Indices buffer
    std::vector<GLfloat> vertices;

    // Load all data from the mesh:
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3D vertex = mesh->mVertices[i];
        buffer.push_back(vertex.x);
        buffer.push_back(vertex.y);
        buffer.push_back(vertex.z);
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);

        aiVector3D uv = mesh->mTextureCoords[0][i];
        buffer.push_back(uv.x);
        buffer.push_back(uv.y);

        if (mesh->HasNormals()) {
            aiVector3D normal = mesh->mNormals[i];
            buffer.push_back(normal.x);
            buffer.push_back(normal.y);
            buffer.push_back(normal.z);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // Set up the game objects rendering through the VAO, VBO, and EBO. Pass in the vertices, normals, uvs
    // and indices to the buffers:
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), &buffer[0], GL_STATIC_DRAW);
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Set the attribute pointers for the buffer as it contains vertices, normals, and uvs all in one array:
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    // Create an empty bounding box and then find the correct opposite corners for the bounding box
    // based on the smallest and largest values in the vertices (positions) array:
    this->boundingBoxMin = glm::vec3(0.0f);
    this->boundingBoxMax = glm::vec3(0.0f);
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
        this->boundingBoxMin = glm::min(this->boundingBoxMin, vertex);
        this->boundingBoxMax = glm::max(this->boundingBoxMax, vertex);
    }
    
    // Save the number of indices and a copy of the objects vertices to the game object
    // (the copy is used later when finding the center of the objects vertices):
    this->indices_count = indices.size();
    this->obj_verts = vertices;

    // Debug print to check vertices and indices counts after loading and processing obj files:
    std::cout << "Number of Vertices: " << buffer.size() / 8 << std::endl;
    std::cout << "Number of Indices: " << indices.size() / 3 << std::endl;
}

// Function to load a texture file and save it to the game object using SOIL2.
void game_object::load_texture(const char* texture_file) {
    // Generate a new texture and bind it to the game objects texture_id:
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    // Set some basic parameters for the texture to keep it looking (hopefully) clean:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Use SOIL2 to load the texture into OpenGL and generate a Mipmap for it:
    int width, height, nrChannels;
    unsigned char* data = SOIL_load_image(texture_file, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(data);
    }
    else
        std::cout << "Failed to load texture: " << texture_file << std::endl;
}

// Function to get the center position of a game object.
glm::vec3 game_object::get_center() {
    glm::vec3 center(0.0f);
    int vertexCount = this->obj_verts.size() / 3;

    for (int i = 0; i < this->obj_verts.size(); i += 3) {
        center.x += this->obj_verts[i];
        center.y += this->obj_verts[i + 1];
        center.z += this->obj_verts[i + 2];
    }

    center.x /= static_cast<float>(vertexCount);
    center.y /= static_cast<float>(vertexCount);
    center.z /= static_cast<float>(vertexCount);
    return center;
}

// Function to get the radius of a game object.
float game_object::get_radius() {
    glm::vec3 center = this->get_center();
    float maxDistance = 0.0f;

    for (int i = 0; i < this->obj_verts.size(); i += 3) {
        float distance = glm::distance(center, glm::vec3(this->obj_verts[i], this->obj_verts[i + 1], this->obj_verts[i + 2]));
        if (distance > maxDistance)
            maxDistance = distance;
    }
    return maxDistance;
}

// Function to render the game object. Will be called in the main game entity manager render loop.
void game_object::render(const glm::vec3& camera_position, const glm::vec3& camera_front, float game_speed, std::vector<glm::vec3> ring_positions[7])
{
    // Initialize debug menu reference:
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Text("%d %f %f %f", this->planet_entity_id, this->position.x, this->position.y, this->position.z);

    // Update the time the object has existed:
    this->time_exist += (glfwGetTime() - last_frame_time) * game_speed;
    this->last_frame_time = glfwGetTime();
    
    // Bind VAO and shader for the current object:
    glBindVertexArray(this->VAO);
    glUseProgram(this->shader);

    // Initialize the model matrix of changes to be made to the object with the scale adjustment:
    glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(this->size_adjust));

    // Every object will orbit around its parent object. (Besides the sun, hence why I start at entity 2) Here is where the 
    // new position of each object on its orbit path is calculated:
    if (this->planet_entity_id > 1 && this->parent != 0)
    {
        // Find the center of the objects parent:
        glm::vec3 parent_planet_center = this->parent_planet->get_center();

        // Find the distance along the orbit path it should be based on the time it has existed (keeps track of 
        // if the game is paused, basically) and the orbit speed of each object:
        this->position.x = parent_planet_center.x + this->orbit_radius * cos(this->time_exist * this->orbit_speed);
        this->position.y = parent_planet_center.y;
        this->position.z = parent_planet_center.z + this->orbit_radius * sin(this->time_exist * this->orbit_speed);

        // Apply the translation to the model matrix and then multiply by the model matrix of its parent
        // so we can achieve accurate sub rotations (i.e. a space station orbiting around a planet that is orbiting the sun):
        model_matrix = glm::translate(model_matrix, glm::vec3(this->position.x, this->position.y, this->position.z));
        model_matrix = this->parent_planet->parent_mm * model_matrix;
    }

    // Add rotation/spinning for all planets (any object higher than entity id 7 will be spawned after the planets):
    if (this->planet_entity_id < 8)
        model_matrix = glm::rotate(glm::mat4(1.0f), this->time_exist * 0.05f, glm::vec3(0.0f, 1.0f, 0.0f)) * model_matrix;

    // If object is a 'preview' object that the player is preparing to place:
    if (this->parent == 0)
    {
        // Get mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(this->window, &mouseX, &mouseY);

        // Convert mouse position to world coordinates
        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        float normalizedX = (2.0f * mouseX) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * mouseY) / screenHeight;

        glm::vec4 ray_clip = glm::vec4(normalizedX, normalizedY, -1.0, 1.0);
        glm::vec4 ray_eye = glm::inverse(projection_matrix) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
        glm::vec4 ray_world = glm::inverse(view_matrix) * ray_eye;
        glm::vec3 ray_direction = glm::normalize(glm::vec3(ray_world));

        float shortest = 999999999.0f;
        glm::vec3 closest_point = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 ray_origin = (camera_position + camera_front);
        for (int i = 0; i < 6; i++) {
            for (const glm::vec3& vertex : ring_positions[i]) {
                // Calculate the intersection point of the ray with the y = 0 plane
                float t = -ray_origin.y / ray_direction.y;
                glm::vec3 intersection_point = ray_origin + t * ray_direction;

                // Calculate the distance between the intersection point and the vertex on the ring:
                float distance = glm::distance(intersection_point, vertex);

                // If this point is closer, update the closest point:
                if (distance < shortest) {
                    shortest = distance;
                    closest_point = vertex;
                }
            }
        }
        // Update the position of the preview object with the new location:
        this->position = closest_point;
        model_matrix = glm::translate(glm::mat4(1.0f), this->position);
    }

    // Find the view matrix and projection with the passed in camera position and camera front references:
    float fov = 45.0f;
    glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection_matrix = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 10000.0f);

    // Pass matrices to the objects shader:
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

    // Load the objects texture and draw the object:
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    glDrawElements(GL_TRIANGLES, this->indices_count, GL_UNSIGNED_INT, 0);

    // Update references of the game object so that sub orbiting objects can find their positions correctly:
    this->parent_mm = model_matrix;
    this->view_matrix = view_matrix;
    this->projection_matrix = projection_matrix;
    this->model_matrix = model_matrix;
    this->position = model_matrix[3];

    // Render the objects bounding box if needed:
    if (this->render_bb)
        this->render_bounding_box();

    glUseProgram(0);
}

// Function to render the bounding box for a game object.
void game_object::render_bounding_box() {
    // Vertices for the objects bounding box cube:
    std::vector<GLfloat> bb_vertices = {
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

    // Set up, draw, and clean up the bounding box render:
    GLuint bb_VAO, bb_VBO;
    // Reuse the game objects shader for simplicity/memory sake, bounding box will roughly same color as the game objects texture:
    glUseProgram(this->shader);
    glGenVertexArrays(1, &bb_VAO);
    glBindVertexArray(bb_VAO);
    glGenBuffers(1, &bb_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bb_VBO);
    glBufferData(GL_ARRAY_BUFFER, bb_vertices.size() * sizeof(GLfloat), &bb_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, bb_vertices.size() / 3);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &bb_VAO);
    glDeleteBuffers(1, &bb_VBO);
    glUseProgram(0);
}

// Class initialization function for orbit path rings.
rings::rings() {
    this->shader = load_shader("shaders/orbit_rings/vert.glsl", "shaders/orbit_rings/frag.glsl");
    glUseProgram(this->shader);

    // Set up orbit path rings VAO and VBO:
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    // Define the vertices for an orbit ring:
    std::vector<float> vertices;
    for (int j = 0; j < 360; j++) {
        float deg = j * 3.14159265359f / 180;
        vertices.push_back(cos(deg));
        vertices.push_back(sin(deg));
        vertices.push_back(0.0f);
    }

    // Fill VBO with vertex data:
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointer:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Clean up:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Function to render the orbit path rings.
void rings::render(const glm::vec3& camera_position, const glm::vec3& camera_front)
{
    glUseProgram(this->shader);
    glBindVertexArray(this->VAO);
    glLineWidth(1.0f);

    // Radiuses for the orbit path ring positions:
    float radiuses[7] = { 200.0 , 322.5, 450.0, 570.0, 742.5, 925.0 };

    // Find the view matrix and projection with the passed in camera position and camera front references:
    float fov = 45.0f;
    glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection_matrix = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 10000.0f);

    // Apply the correct translations for each of the seven rings and draw it:
    for (int i = 0; i < 6; i++)
    {
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(radiuses[i], radiuses[i], radiuses[i]));

        // Save location of every vertex in the orbit rings for use later in object placement:
        if (this->ring_positions[i].empty())
        {
            for (int j = 0; j < 360; j++) {
                glm::vec4 vertex = model * glm::vec4(cos(glm::radians(static_cast<float>(j))), sin(glm::radians(static_cast<float>(j))), 0.0f, 1.0f);
                glm::vec3 transformedPosition = glm::vec3(vertex.x, vertex.y, vertex.z);
                this->ring_positions[i].push_back(transformedPosition);
            }
        }

        glUniformMatrix4fv(glGetUniformLocation(this->shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniformMatrix4fv(glGetUniformLocation(this->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_LINE_LOOP, 0, 360);

    }
    glUseProgram(0);

}

// Function to read in the text of a shader file.
GLchar* read_shader_text(const char* file_path)
{
    FILE* shader_file;
    errno_t error = fopen_s(&shader_file, file_path, "rb");
    if (error == '0' || !shader_file)
    {
        std::cout << "Error in opening shader: " << file_path << "." << std::endl;
        fclose(shader_file);
        return NULL;
    }
    // Find the length of the shader file and allocate memory to the shader char array:
    fseek(shader_file, 0, SEEK_END);
    long int file_len = ftell(shader_file);
    rewind(shader_file);
    GLchar* shader_content = (GLchar*)malloc(file_len + 1);

    // Read the contents of the shader file, remove the empty char at the end, close the file, and return the text:
    fread(shader_content, 1, file_len, shader_file);
    fclose(shader_file);
    shader_content[file_len] = '\0';
    return shader_content;
}

// Function to load vertex and fragment shaders.
GLuint load_shader(const char* vertex_file_path, const char* frag_file_path)
{
    // Load in text from the frag and vertex shaders:
    GLchar* vertex_shader_content = read_shader_text(vertex_file_path);
    GLchar* frag_shader_content = read_shader_text(frag_file_path);

    // Create the vertex shader in OpenGL, pass it the correct source file, compile it, and free the char array of the input file:
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
    glCompileShader(vertex_shader);
    free(vertex_shader_content);

    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "Vertex shader " << vertex_file_path << " compilation failed : " << infoLog << std::endl;
    }

    // Repeat for frag shader:
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag_shader_content, NULL);
    glCompileShader(fragment_shader);
    free(frag_shader_content);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cerr << "Fragment shader" << frag_file_path << "compilation failed: " << infoLog << std::endl;
    }

    // Create shader program with the now compiled vertex and fragment shaders:
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Delete the old individual vertex and fragment shaders and return the compiled shader program:
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}