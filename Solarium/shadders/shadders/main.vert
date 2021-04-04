#version 460 core

// ##### UNIFORMS #####
uniform vec3 cameraPosition;

// ##### INPUT LAYOUT #####
layout (location = 0) in vec3 raydir;

// ##### SHADER SETUP #####
out vec3 campos;
out vec3 raydirection;

void main(){
    // Output to the Fragment Shader
    campos = CameraPosition;
    raydirection = raydir;

    // Output Vertex Shader Crap
    //gl_Position = vec4(position, 1.0);
}