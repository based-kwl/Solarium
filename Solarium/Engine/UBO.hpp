#pragma once
#include "SwapChain.hpp"
#include "BufferHelper.hpp"
#include "Device.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <memory>
#include <chrono>
#include "vulkan/vulkan.hpp"

namespace Solarium
{

	class UBO
	{
	public:
		UBO(SwapChain* swapChain, Device* device, vk::Sampler textureSampler, vk::ImageView textureImageView);
		~UBO();
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		void updateUniformbuffer(uint32_t currentImage, SwapChain* swapChain, Device* device);
		vk::DescriptorPool getDescriptorPool() { return descriptorPool; }
		std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
		vk::DescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }
		std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }
		std::vector<vk::DeviceMemory> getUniformBuffersMemory() { return uniformBuffersMemory; }

	private:
		void createUniformBuffers(SwapChain* swapChain, Device* device);
		void createDescriptorPool(SwapChain* swapChain, Device* device);
		void createDescriptorSets(SwapChain* swapChain, Device* device, vk::Sampler textureSampler, vk::ImageView textureImageView);
		vk::DescriptorPool descriptorPool;
		std::vector<vk::DescriptorSet> descriptorSets;
		vk::DescriptorSetLayout descriptorSetLayout;
		std::vector<vk::Buffer> uniformBuffers;
		std::vector<vk::DeviceMemory> uniformBuffersMemory;
		struct UniformBufferObject {
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
		};
	};
}