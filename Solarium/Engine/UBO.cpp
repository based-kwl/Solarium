#include "UBO.hpp"


namespace Solarium
{
	UBO::UBO(SwapChain* swapChain_, Device* device_) {
		swapChain = swapChain_;
		device = device_;
		createDescriptorSetLayout();
	}

	void UBO::createDescriptorSetLayout()
	{
		vk::DescriptorSetLayoutBinding uboLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };
		vk::DescriptorSetLayoutBinding samplerLayoutBinding{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };
		std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		vk::DescriptorSetLayoutCreateInfo layoutInfo{ {}, bindings };

		descriptorSetLayout = device->device().createDescriptorSetLayout(layoutInfo);
	}

	void UBO::createChain(vk::Sampler textureSampler, vk::ImageView textureImageView) {
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets(textureSampler, textureImageView);
	}

	void UBO::createUniformBuffers()
	{
		vk::DeviceSize deviceSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(swapChain->imageCount());
		uniformBuffersMemory.resize(swapChain->imageCount());

		for (size_t i = 0; i < swapChain->imageCount(); i++)
		{
			BufferHelper::createBuffer(deviceSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i], device);
		}
	}


	void UBO::updateUniformbuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->width() / (float)swapChain->height(), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		void* data = device->device().mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo));
		memcpy(data, &ubo, sizeof(ubo));
		device->device().unmapMemory(uniformBuffersMemory[currentImage]);
	}


	void UBO::createDescriptorPool()
	{
		std::array<vk::DescriptorPoolSize, 2> poolSizes{ vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(swapChain->imageCount())}, vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(swapChain->imageCount())} };
		vk::DescriptorPoolCreateInfo poolInfo{ {}, static_cast<uint32_t>(swapChain->imageCount()), poolSizes };

		descriptorPool = device->device().createDescriptorPool(poolInfo);
		if (!descriptorPool)
		{
			throw std::runtime_error("Failed to create descriptor pool");
		}
	}


	void UBO::createDescriptorSets(vk::Sampler textureSampler, vk::ImageView textureImageView)
	{
		std::vector<vk::DescriptorSetLayout> layouts(swapChain->imageCount(), descriptorSetLayout);
		vk::DescriptorSetAllocateInfo allocInfo{ descriptorPool, layouts };

		descriptorSets.resize(swapChain->imageCount());
		descriptorSets = device->device().allocateDescriptorSets(allocInfo);
		if (descriptorSets[0] == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < swapChain->imageCount(); i++)
		{
			vk::DescriptorBufferInfo bufferInfo{ uniformBuffers[i], 0, sizeof(UniformBufferObject) };
			vk::DescriptorImageInfo imageInfo{ textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };
			std::array<vk::WriteDescriptorSet, 2> descriptorWrites{ vk::WriteDescriptorSet{descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufferInfo}, vk::WriteDescriptorSet{descriptorSets[i], 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo } };
			device->device().updateDescriptorSets(descriptorWrites, 0);
		}
	}
}