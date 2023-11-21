// Planet vert shader

#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;     // UVs at attribute location 1
layout(location = 2) in vec3 vertexNormal; // Normals at attribute location 2

out vec2 fragUV;
out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragUV = vertexUV;
    fragNormal = normalize(mat3(transpose(inverse(model))) * vertexNormal);
    fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}