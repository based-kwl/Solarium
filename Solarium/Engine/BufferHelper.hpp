#pragma once

#include <vulkan/vulkan.hpp>
#include "Device.hpp"
namespace Solarium
{

	class BufferHelper
	{
	public:

		static void createBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::Buffer& buffer,
			vk::DeviceMemory& bufferMemory,
			Device* device);
		static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device* device);
		static void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size, Device* device);
		static vk::CommandBuffer beginSingleTimeCommands(Device* device);
		static void endSingleTimeCommands(vk::CommandBuffer commandBuffer, Device* device);
		static void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, Device* device);
	private:
		
	};
}