#version 330

in vec3 position;
out vec3 frag_tex_coord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);
    frag_tex_coord = position;
}