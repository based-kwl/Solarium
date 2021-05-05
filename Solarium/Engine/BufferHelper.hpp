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
	private:
		static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device* device);
	};
}