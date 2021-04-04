#pragma once

#include <vulkan/vulkan.hpp>
#include "Platform.hpp"
#include "GLFW/glfw3.h"

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