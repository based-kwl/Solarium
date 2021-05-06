#include "UBO.hpp"


namespace Solarium
{
	UBO::UBO(SwapChain* swapChain_, Device* device_) {
		swapChain = swapChain_;
		device = device_;
		depositDescriptorSetBinding({ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex });
		depositDescriptorSetBinding({ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment });
		depositDescriptorSetBinding({ 2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex });
		createDescriptorSetLayout();
	}

	void UBO::createDescriptorSetLayout()
	{
		vk::DescriptorSetLayoutCreateInfo layoutInfo{ {}, descriptorSetLayoutBindings };

		descriptorSetLayout = device->device().createDescriptorSetLayout(layoutInfo);
	}

	void UBO::createChain(vk::Sampler textureSampler, vk::ImageView textureImageView) {
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets(textureSampler, textureImageView);
	}

	void UBO::createUniformBuffers()
	{
		vk::DeviceSize deviceSize = sizeof(structUBOmvp);
		vk::DeviceSize secDeviceSize = sizeof(structUBOtest);


		UBOtest.resize(swapChain->imageCount());
		UBOmvp.resize(swapChain->imageCount());
		UBOtestMemory.resize(swapChain->imageCount());
		UBOmvpMemory.resize(swapChain->imageCount());

		for (size_t i = 0; i < swapChain->imageCount(); i++)
		{
			BufferHelper::createBuffer(deviceSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, UBOtest[i], UBOtestMemory[i], device);
			BufferHelper::createBuffer(deviceSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, UBOmvp[i], UBOmvpMemory[i], device);
		}
	}

	void UBO::updateUniformbuffer(uint32_t currentImage, UBOType type, UBOlist ubolist)
	{
		switch (type)
		{
			case (UBOType::MVP):
			{
				void* data = device->device().mapMemory(UBOmvpMemory[currentImage], 0, sizeof(ubolist.mvp));
				memcpy(data, &ubolist.mvp, sizeof(ubolist.mvp));
				device->device().unmapMemory(UBOmvpMemory[currentImage]);
				break;
			}
			case (UBOType::COLOR):
			{
				void* data = device->device().mapMemory(UBOtestMemory[currentImage], 0, sizeof(ubolist.test));
				memcpy(data, &ubolist.test, sizeof(ubolist.test));
				device->device().unmapMemory(UBOtestMemory[currentImage]);
				break;
			}
		}
		
	}

	void UBO::createDescriptorPool()
	{
		std::array<vk::DescriptorPoolSize, 3> poolSizes{ vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(swapChain->imageCount())}, vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(swapChain->imageCount())}, vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(swapChain->imageCount())} };
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
			vk::DescriptorBufferInfo bufferInfo{ UBOmvp[i], 0, sizeof(structUBOmvp) };
			vk::DescriptorBufferInfo bufferInfo2{ UBOtest[i], 0, sizeof(structUBOtest) };
			vk::DescriptorImageInfo imageInfo{ textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };
			std::array<vk::WriteDescriptorSet, 3> descriptorWrites{ vk::WriteDescriptorSet{descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufferInfo}, vk::WriteDescriptorSet{descriptorSets[i], 2, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufferInfo2}, vk::WriteDescriptorSet{descriptorSets[i], 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo } };
			device->device().updateDescriptorSets(descriptorWrites, 0);
		}
	}
}