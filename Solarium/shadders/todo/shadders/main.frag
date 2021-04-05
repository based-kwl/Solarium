#version 460 core

// ##### UNIFORMS #####
uniform vec2 resolution;
uniform vec3 campos;
uniform vec3 camdir;

// ##### SHADER SETUP #####
in vec3 campos;
in vec3 raydirection;
layout (location = 0) out vec4 fragColor;

// ##### HEADERS #####
#include "common.glsl"
#include "parameters.glsl"

// ##### DISTANCE ESTIMATORS #####
// Planet Distance Estimator
float planet(vec3 pos){
    return length(pos)-0.5;
}

// Ray-March the Planet
float intersectPlanet(in vec3 rayori, in vec3 raydir, out vec3 albedo, out vec3 norm, out float rough){
    float dist = 0.0;
    for(uint i = 0U; i < maxmarches; i++){
        if(length((rayori+(raydir*dist))-rayori) > maxdist){break;}
        float distest = planet(rayori+(raydir*dist));
        if(distest < collisiondist){return dist}
        dist += distest;
    }
    return -1.0;
}

// ##### RENDERING #####
float intersect(in vec3 rayori, in vec3 raydir, out vec3 albedo, out vec3 norm, out float rough){
    // Find all of the Intersections
    float intersection0 = intersectPlanet(rayori, raydir, albedo, norm, rough);

    // Planet
    return intersection0;

    // No Intersection
    return -1.0;
}

#ifndef pathtracing
// Real-Time Physically-Based Shading
vec3 shade(vec3 rayori, vec3 raydir, vec3 raypos, vec3 abledo, vec3 norm, float rough){
    //vec3 shadeggx = ggx();
    return vec3(0.5);
}
#endif

vec3 skyCol(vec3 dir){
    return vec3(0.5);
}

// Ray-Tracing
vec3 raytrace(){
    // Initialize Variables
    vec3 raydir = camdir, raypos = campos;
    float intersection;

    // Material Properties
    vec3 attenuation = vec3(1.0), abledo, norm;
    float rough;

    // Ray-Tracing Loop
    for(uint i = 0U; i < maxbounces; i++){
        // Find the Intersection of the Scene
        intersection = intersect(raypos, raydir, albedo, norm, rough);

        // If the Intersection is less than 0.0, the ray didn't hit anything
        if(intersection < 0.0){break;}

        #ifndef pathtracing
        // If the Object is Rough, Use Real-Time Shading
        if(rough > 0.01){
            return attenuation*shade(raypos, raydir, raypos+raydir*intersection, abledo, norm, rough);
        }
        #endif

        raypos += raydir*intersection;
        attenuation *= albedo;
        raydir = reflect(raydir, normal);
    }

    // Return the Ray-Traced Pixel
    return attenuation*skyCol(raydir);
}

// Render and Output the Frame
void main(){
    // Screen UV Coordinates
    //vec2 uv = 2.0*(gl_FragCoord.xy-0.5*resolution.xy)/max(resolution.x, resolution.y);

    // Render the Frame
    vec3 color = raytrace();

    // HDR Tonemapping
    color = pow(color/(color+1.0), vec3(1.0/gamma));

    // Output the Rendered Frame
    fragColor = vec4(color, 1.0);
}