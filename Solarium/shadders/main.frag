#version 460 core

// ##### SHADER SETUP #####
layout (location = 0) out vec4 fragColor;
/*layout (binding = 1, std140) uniform windowParameters
{
	vec2 resolution;
};*/

// Debug, do not actually trust that the resolution is 800x600 all of the time.
#define resolution vec2(800.0, 600.0)

// Render and Output the Frame
void main()
{
	// Screen UV Coordinates
	vec2 uv = 2.0*gl_FragCoord.xy/max(resolution.x, resolution.y);

	// Render the Frame
	vec3 color = vec3(uv, 0.0);

	// Output the Rendered Frame
	fragColor = vec4(color, 1.0);
}