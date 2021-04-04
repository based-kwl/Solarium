#version 460 core

// ##### SHADER SETUP #####
in vec3 campos;
in vec3 raypos;
in vec3 normal;
out vec4 fragColor;

// ##### HEADERS #####
#include "common.glsl"
#include "parameters.glsl"

void main(){
    fragColor = vec4(color, 1.0);
}