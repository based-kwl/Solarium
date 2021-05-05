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
		UBO(SwapChain* swapChain_, Device* device_);
		~UBO();
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		void updateUniformbuffer(uint32_t currentImage);
		vk::DescriptorPool getDescriptorPool() { return descriptorPool; }
		std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
		vk::DescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }
		std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }
		std::vector<vk::DeviceMemory> getUniformBuffersMemory() { return uniformBuffersMemory; }
		void createChain(vk::Sampler textureSampler, vk::ImageView textureImageView);

		void update(SwapChain* swapChain_, Device* device_) { swapChain = swapChain_; device = device_; }

	private:
		Device* device;
		SwapChain* swapChain;
		void createDescriptorSetLayout();
		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSets(vk::Sampler textureSampler, vk::ImageView textureImageView);
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