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
	typedef enum class UBOType{
		MVP,
		COLOR
	}UBOType;

	struct structUBOmvp {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct structUBOtest {
		alignas(16) glm::mat4 test;
	};

	struct UBOlist {
		structUBOmvp mvp;
		structUBOtest test;
	};

	class UBO
	{
	public:
		UBO(SwapChain* swapChain_, Device* device_);
		~UBO();
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		void updateUniformbuffer(uint32_t currentImage, UBOType type, UBOlist ubolist);
		vk::DescriptorPool getDescriptorPool() { return descriptorPool; }
		std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
		vk::DescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }

		std::vector<vk::Buffer> getUniformBuffers(UBOType type)
		{
			switch (type)
			{
				case(UBOType::MVP):
				{
					return UBOmvp;
				}
				case(UBOType::COLOR):
				{
					return UBOtest;
				}
			}
		}

		void depositDescriptorSetBinding(vk::DescriptorSetLayoutBinding binding) { descriptorSetLayoutBindings.push_back(binding); }
		std::vector<vk::DeviceMemory> getUniformBuffersMemory(UBOType type)
		{
			switch (type)
			{
			case(UBOType::MVP):
			{
				return UBOmvpMemory;
			}
			case(UBOType::COLOR):
			{
				return UBOtestMemory;
			}
			}
		}
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
		std::vector <vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
		vk::DescriptorSetLayout descriptorSetLayout;
		std::vector<vk::Buffer> UBOmvp;
		std::vector<vk::Buffer> UBOtest;
		std::vector<vk::DeviceMemory> UBOmvpMemory;
		std::vector<vk::DeviceMemory> UBOtestMemory;
	};
}