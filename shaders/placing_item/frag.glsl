// Temp frag shader

#version 330

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

uniform float rotation_angle;
uniform sampler2D textureSampler;
uniform bool ok_place;

void main() {
    vec3 norm = normalize(fragNormal);

    mat2 rotationMatrix = mat2(cos(rotation_angle), -sin(rotation_angle),
                               sin(rotation_angle), cos(rotation_angle));

    vec2 rotatedTexCoord = rotationMatrix * fragUV;

    vec4 texColor = texture(textureSampler, rotatedTexCoord);
    
    if (ok_place == false)
    {
        // Apply red tint
        texColor.r += 0.5;
        texColor.g -= 0.25;
        texColor.b -= 0.25;
    }

    else
    {
        texColor.r = 1.0;
        texColor.g = 1.0;
        texColor.b = 1.0;
    }
    
    // Apply transparency
  //  texColor.a *= 0.5;

    fragColor = texColor;
}
