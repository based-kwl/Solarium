#version 460 core

// ##### SHADER SETUP #####
in vec3 norm;
layout (location = 0) out vec4 fragColor;

// Render and Output the Frame
void main(){
    vec3 lightdir = normalize(vec3(1.0));
    float shaded = max(dot(norm, lightdir), 0.0);

    // Output the Rendered Frame
    fragColor = vec4(shaded, 1.0);
}