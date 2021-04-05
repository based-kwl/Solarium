#version 460 core
#extension GL_KHR_vulkan_glsl : enable

// Swizzle K for Smaller Code when Dealing with Screen Corners
const vec2 k = vec2(-1.0, 1.0);

// Screen Vertex Coordinates
vec2 positions[6] = vec2[]
(
	k.xx,
	k.yx,
	k.xy,
	k.yy,
	k.yx,
	k.xy
);

// Output the Vertex Data
void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}