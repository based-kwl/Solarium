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
		VIEWMODEL,
		CAMERA
	}UBOType;

	struct structUBOviewmodel {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec3 rotation;
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec4 viewPos;
	};

	struct structUBOcamera {
		alignas(16) glm::vec3 rotation;
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec4 viewPos;
	};

	struct UBOlist {
		structUBOviewmodel viewmodel;
		structUBOcamera camera;
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
				case(UBOType::VIEWMODEL):
				{
					return UBOviewmodel;
				}
				case(UBOType::CAMERA):
				{
					return UBOcamera;
				}
			}
		}

		void depositDescriptorSetBinding(vk::DescriptorSetLayoutBinding binding) { descriptorSetLayoutBindings.push_back(binding); }
		std::vector<vk::DeviceMemory> getUniformBuffersMemory(UBOType type)
		{
			switch (type)
			{
			case(UBOType::VIEWMODEL):
			{
				return UBOviewmodelMemory;
			}
			case(UBOType::CAMERA):
			{
				return UBOcameraMemory;
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
		std::vector<vk::Buffer> UBOviewmodel;
		std::vector<vk::Buffer> UBOcamera;
		std::vector<vk::DeviceMemory> UBOviewmodelMemory;
		std::vector<vk::DeviceMemory> UBOcameraMemory;
	};
}