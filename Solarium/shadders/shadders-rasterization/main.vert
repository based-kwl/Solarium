#version 460 core

in vec3 campos;
in vec3 raypos;
in vec3 normal;
out vec4 fragColor;

void main(){
    fragColor = vec4(color, 1.0);
}