#pragma once

#include "BufferHelper.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <memory>

namespace Solarium
{

	class VertexBuffer
	{
	public:
		VertexBuffer(Device* device_);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		vk::Buffer getVertexBuffer() { return vertexBuffer; }
		vk::Buffer getIndexBuffer() { return indexBuffer; }
		vk::DeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }
		vk::DeviceMemory getIndexBufferMemory() { return indexBufferMemory; }
		void update(SwapChain* swapChain_, Device* device_) { device = device_; }

		void createChain();

		const std::vector<Vertex> vertices = {
{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

	private:
		Device* device;
		void createVertexBuffer();
		void createIndexBuffer();
		vk::Buffer vertexBuffer;
		vk::Buffer indexBuffer;
		vk::DeviceMemory vertexBufferMemory;
		vk::DeviceMemory indexBufferMemory;

	};
}