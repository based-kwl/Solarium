#include "BufferHelper.hpp"

namespace Solarium
{
	void BufferHelper::createBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory,
		Device* device)
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		buffer = device->device().createBuffer(bufferInfo);

		if (!buffer)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements = device->device().getBufferMemoryRequirements(buffer);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

		bufferMemory = device->device().allocateMemory(allocInfo);

		if (VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		device->device().bindBufferMemory(buffer, bufferMemory, 0);
	}

	uint32_t BufferHelper::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device* device)
	{
		vk::PhysicalDeviceMemoryProperties memProperties = device->physicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type");
	}

	void BufferHelper::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size, Device* device)
	{
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device);
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy{ {}, {}, size });
		endSingleTimeCommands(commandBuffer, device);
	}


	vk::CommandBuffer BufferHelper::beginSingleTimeCommands(Device* device)
	{
		vk::CommandBufferAllocateInfo allocInfo{ device->getCommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
		vk::CommandBuffer commandBuffer = device->device().allocateCommandBuffers(allocInfo)[0];
		vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		commandBuffer.begin(beginInfo);
		return commandBuffer;
	}


	void BufferHelper::endSingleTimeCommands(vk::CommandBuffer commandBuffer, Device* device)
	{
		commandBuffer.end();
		device->graphicsQueue().submit(vk::SubmitInfo{ {}, {}, commandBuffer });
		device->graphicsQueue().waitIdle();
		device->device().freeCommandBuffers(device->getCommandPool(), commandBuffer);
	}


	void BufferHelper::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, Device* device)
	{
		vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device);
		vk::BufferImageCopy region{ 0, 0, 0, vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, { width, height, 1 } };
		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
		endSingleTimeCommands(commandBuffer, device);
	}
}