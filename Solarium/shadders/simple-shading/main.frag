#version 460 core

in vec3 norm;
out vec4 fragColor;

void main(){
    vec3 lightdir = normalize(vec3(1.0));
    float shaded = max(dot(norm, lightdir), 0.0);
    fragColor = vec4(shaded, 1.0);
}