#pragma once

#include "../Typedef.h"
#include "Device.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <memory>

namespace Solarium
{

	class Platform;
	class Engine
	{
	public:
		Engine(const char* applicationName, uint32_t width, uint32_t height);
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Run();

		void OnLoop(const uint32_t deltaTime);

		bool getFramebufferResized() { return framebufferResized; }
		void setFramebufferResized(bool resized) { framebufferResized = resized; }
	private:
		void createBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::Buffer& buffer,
			vk::DeviceMemory& bufferMemory);
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

		void createPipelineLayout();
		void createPipeline();
		void createVertexBuffer();
		void createIndexBuffer();
		void createCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void cleanupSwapChain();

		void cleanup();

		Platform* _platform;
		Device* device;
		SwapChain* swapChain;
		Pipeline* pipeline;
		vk::PipelineLayout pipelineLayout;
		std::vector<vk::CommandBuffer> commandBuffers;
		vk::DeviceMemory vertexBufferMemory;
		vk::Buffer vertexBuffer;
		vk::Buffer indexBuffer;
		vk::DeviceMemory indexBufferMemory;
		bool framebufferResized = false;
	};
}