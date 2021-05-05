#pragma once

#include <chrono>

#include "../Typedef.h"
#include "Device.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "UBO.hpp"

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
		vk::CommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
		void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

		void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		void createPipelineLayout();
		void createDescriptorSetLayout();
		void createPipeline();
		vk::ImageView createImageView(vk::Image image, vk::Format format);
		void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
		void createTextureImage();
		void createImageViews();
		void createTextureImageView();
		void createTextureSampler();
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
		UBO* uniformBufferObject;
		vk::PipelineLayout pipelineLayout;
		std::vector<vk::CommandBuffer> commandBuffers;
		vk::DeviceMemory vertexBufferMemory;
		std::vector<vk::ImageView> swapChainImageViews;
		vk::Buffer vertexBuffer;
		vk::Buffer indexBuffer;
		vk::Sampler textureSampler;
		vk::DeviceMemory indexBufferMemory;
		vk::ImageView textureImageView;
		vk::Image textureImage;
		vk::DeviceMemory textureImageMemory;
		bool framebufferResized = false;
	};
}