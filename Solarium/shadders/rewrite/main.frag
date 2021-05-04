#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 baseColor;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 color = vec3(0.0);

	fragColor = vec4(color, 1.0);
}