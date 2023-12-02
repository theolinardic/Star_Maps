#version 330

in vec2 TexCoord;
out vec4 out_color;

uniform sampler2D text;

void main()
{
    out_color = texture(text, TexCoord);
}