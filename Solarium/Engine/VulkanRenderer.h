#pragma once

#include <vulkan/vulkan.h>

namespace Solarium
{
	class Platform;

	class VulkanRenderer
	{
	public:
		VulkanRenderer(Platform* platform);
		~VulkanRenderer();
	private:
		Platform* _platform;
	};


}