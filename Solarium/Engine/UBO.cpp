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
		vk::DeviceSize deviceSize = sizeof(structUBOviewmodel);
		vk::DeviceSize secDeviceSize = sizeof(structUBOcamera);


		UBOcamera.resize(swapChain->imageCount());
		UBOviewmodel.resize(swapChain->imageCount());
		UBOcameraMemory.resize(swapChain->imageCount());
		UBOviewmodelMemory.resize(swapChain->imageCount());

		for (size_t i = 0; i < swapChain->imageCount(); i++)
		{
			BufferHelper::createBuffer(deviceSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, UBOcamera[i], UBOcameraMemory[i], device);
			BufferHelper::createBuffer(deviceSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, UBOviewmodel[i], UBOviewmodelMemory[i], device);
		}
	}

	void UBO::updateUniformbuffer(uint32_t currentImage, UBOType type, UBOlist ubolist)
	{
		switch (type)
		{
			case (UBOType::VIEWMODEL):
			{
				void* data = device->device().mapMemory(UBOviewmodelMemory[currentImage], 0, sizeof(ubolist.viewmodel));
				memcpy(data, &ubolist.viewmodel, sizeof(ubolist.viewmodel));
				device->device().unmapMemory(UBOviewmodelMemory[currentImage]);
				break;
			}
			case (UBOType::CAMERA):
			{
				void* data = device->device().mapMemory(UBOcameraMemory[currentImage], 0, sizeof(ubolist.camera));
				memcpy(data, &ubolist.camera, sizeof(ubolist.camera));
				device->device().unmapMemory(UBOcameraMemory[currentImage]);
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
			vk::DescriptorBufferInfo bufferInfo{ UBOviewmodel[i], 0, sizeof(structUBOviewmodel) };
			vk::DescriptorBufferInfo bufferInfo2{ UBOcamera[i], 0, sizeof(structUBOcamera) };
			vk::DescriptorImageInfo imageInfo{ textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };
			std::array<vk::WriteDescriptorSet, 3> descriptorWrites{ vk::WriteDescriptorSet{descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufferInfo}, vk::WriteDescriptorSet{descriptorSets[i], 2, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufferInfo2}, vk::WriteDescriptorSet{descriptorSets[i], 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo } };
			device->device().updateDescriptorSets(descriptorWrites, 0);
		}
	}
}