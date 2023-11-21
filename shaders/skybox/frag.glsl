#version 330

in vec3 frag_tex_coord;
out vec4 out_color;
uniform samplerCube skybox;

void main()
{
    out_color = texture(skybox, frag_tex_coord);
}