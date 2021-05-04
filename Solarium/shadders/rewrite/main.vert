#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 baseColor;

void main()
{
	// Fragment Shader Data
	pos = position;
	baseColor = vec3(0.8, 0.8, 0.8);

	// Vertex Shader Data
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
}