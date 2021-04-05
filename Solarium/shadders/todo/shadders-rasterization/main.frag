#version 460 core

// ##### SHADER SETUP #####
in vec3 campos;
in vec3 raypos;
in vec3 normal;
layout (location = 0) out vec4 fragColor;

// ##### HEADERS #####
#include "common.glsl"
#include "parameters.glsl"

// ##### SHADING #####
// Trowbridge-Reitz GGX Normal Distribution Function
float GGX(vec3 norm, vec3 highlightdir, float rough){
    float a2     = rough*rough;
    float NdotH  = max(dot(norm, highlightdir), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom    = a2;
    float denom  = (NdotH2*(a2-1.0)+1.0);
          denom  = pi*denom*denom;
    return nom/denom;
}

// Fresnel Schlick
vec3 fresnel(vec3 raydir, vec3 norm, vec3 F0){
    return F0+(1.0-F0)*pow(1.0-dot(-raydir, norm), 5.0);
}

// ##### RENDERING #####
// Render and Output the Frame
void main(){
    // Lighting Variables
    vec3 lightdir = normalize(vec3(1.0));
    vec3 raydir = normalize(raypos-campos);

    // Calculate Shading
    float shadeGGX = GGX(reflect(raydir, normal), lightdir, 0.5);
    vec3  shadefresnel = fresnel(raydir, normal, vec3(0.5));

    // Render the Frame
    vec3 color = shadefresnel*shadeGGX;

    // HDR Tonemapping
    color = pow(color/(color+1.0), vec3(1.0/gamma));

    // Output the Rendered Frame to the Screen
    fragColor = vec4(color, 1.0);
}