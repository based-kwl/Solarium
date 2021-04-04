#version 460 core

// ##### UNIFORMS #####
uniform mat4 matVP;
uniform mat4 matGeo;
uniform vec3 cameraPosition;

// ##### INPUT LAYOUT #####
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
//layout (location = 2) in vec3 texturecoord;

// ##### SHADER SETUP #####
out vec3 campos;
out vec3 raypos;
out vec3 normal;

void main(){
    // Output to the Fragment Shader
    normal = norm;
    campos = CameraPosition3;
    raypos = position;

    // Output Vertex Shader Crap
    gl_Position = matVP*matGeo*vec4(position, 1.0);
}