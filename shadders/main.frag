// ##### UNIFORMS #####
uniform vec2 resolution;

// ##### CONSTANTS #####
#define pi 3.14159265

// ##### PARAMETERS #####
#define gamma 2.2

// ##### RENDERING #####

// Render and Output the Frame
void main(){
    // Screen UV Coordinates
    vec2 uv = 2.0*(gl_FragCoord.xy-0.5*resolution.xy)/max(resolution.x, resolution.y);

    // Render the Frame
    vec3 color = ;

    // HDR Tonemapping
    color = pow(color/(color+1.0), vec3(1.0/gamma));

    // Output the Rendered Frame
    gl_FragColor = vec4(color, 1.0);
}