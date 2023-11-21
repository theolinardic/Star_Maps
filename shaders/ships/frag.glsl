// Ship frag shader

#version 330

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

uniform float rotation_angle;
uniform sampler2D textureSampler;

void main() {
    vec3 norm = normalize(fragNormal);

    vec3 object_color = texture(textureSampler, fragUV).xyz;
    fragColor = vec4(object_color, 1.0f);
    //fragColor = vec4(vec3(1.0,0.5,0.2), 1.0);


   // vec3 ambientLight = vec3(0.3, 0.3, 0.3); // Ambient light color
   // vec3 lightDirection = normalize(vec3(0.5, 0.5, 1.0)); // Directional light direction

   // vec3 texColor = texture(textureSampler, fragUV).rgb; // Sample the texture

  //  vec3 normal = normalize(fragNormal);
  //  vec3 modifiedNormal = texture(textureSampler, fragUV).rgb * 2.0 - 1.0;
   // vec3 adjustedNormal = normalize(fragNormal + modifiedNormal);
   // float diffIntensity = max(dot(adjustedNormal, -lightDirection), 0.0);
   // vec3 diffuse = diffIntensity * vec3(1.0, 1.0, 1.0); // Diffuse light color
   // vec3 finalColor = (ambientLight + diffuse) * texColor; // Final color with lighting
   // fragColor = vec4(finalColor, 1.0);


    //float diffIntensity = max(dot(normal, -lightDirection), 0.0);
    //vec3 diffuse = diffIntensity * vec3(1.0, 1.0, 1.0); // Diffuse light color
    

    //vec3 finalColor = (ambientLight + diffuse) * texColor; // Final color with lighting
    //fragColor = vec4(finalColor, 1.0);
    //fragColor = texture(textureSampler, fragUV); // Uncomment this line to use only texture without lighting
    //fragColor = vec4(fragUV, 0.0, 1.0);
}
