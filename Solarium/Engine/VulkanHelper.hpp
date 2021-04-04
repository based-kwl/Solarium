#include <iostream>
#include <GLFW/glfw3.h>
#include "Logger.hpp"
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace Solarium
{
	class VulkanHelper
	{
	public:
        static void InitializeInstance(GLFWwindow* window, vk::Instance instance);

	};


}