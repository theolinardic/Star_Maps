#include <rings.h>

const float PI = 3.14159265359f;

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

    float radiuses[7] = { 400.0 , 645.0, 900.0, 1140.0, 1500.0, 1700.0, 1900.0 };

    // Incremental scaling for each ring
    float scaleFactor = 1.0f;
    for (int i = 0; i < 7; i++)
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
