#pragma once

#define GFLW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Platform.hpp"

namespace Solarium
{
	class Platform;

	class VulkanRenderer
	{
	public:
		VulkanRenderer(Platform* platform, vk::Instance instance);
		~VulkanRenderer();

	private:
		Platform* _platform;

	};
}