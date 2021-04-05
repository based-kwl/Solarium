#version 460 core

// ##### PARAMETERS #####
// Image Gamma
#define gamma 2.2
#define camfov 1.0
#define maxmarches 128U
#define maxdist 32.0
#define collisiondist 0.01

// ##### SHADER SETUP #####
layout (location = 0) out vec4 fragColor;
/*layout (binding = 1, std140) uniform windowParameters
{
	vec2 resolution;
};*/

// Debug, do not actually trust that the resolution is 800x600 all of the time.
#define resolution vec2(800.0, 600.0)

float de(in vec3 pos, out vec3 norm)
{
	norm = normalize(pos);
	return length(pos)-0.5;
}

vec3 raymarch(vec3 raydir, vec3 rayori)
{
	vec3 raypos = rayori;
	vec3 normal;
	for(uint i = 0U; i < maxmarches; i++)
	{
		if(length(raypos-rayori) > maxdist){break;}
		float distest = de(raypos, normal);
		if(distest < collisiondist)
		{
			return vec3(max(dot(normal, normalize(vec3(1.0))), 0.0));
		}
		raypos += raydir*distest;
	}
	return vec3(0.0);
}

// Render and Output the Frame
void main()
{
	// Screen UV Coordinates
	vec2 uv = 2.0*(gl_FragCoord.xy-0.5*resolution.xy)/max(resolution.x, resolution.y);

	vec3 raydir = normalize(vec3(camfov*uv, 1.0));

	// Render the Frame
	vec3 color = raymarch(raydir, vec3(0.0, 0.0, -2.0));

	// HDR Tonemapping
	color = pow(color/(color+1.0), vec3(1.0/gamma));

	// Output the Rendered Frame
	fragColor = vec4(color, 1.0);
}