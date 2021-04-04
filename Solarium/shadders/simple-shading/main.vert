#version 460 core

uniform mat4 matVP;
uniform mat4 matGeo;

layout (location = 0) in vec3 raypos;
layout (location = 1) in vec3 normal;

out vec3 norm;

void main(){
    norm = normal.xyz;
    gl_Position = matVP*matGeo*vec4(raypos, 1.0);
}