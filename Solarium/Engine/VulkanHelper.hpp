#include <iostream>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Logger.hpp"
namespace Solarium
{
	

	class VulkanHelper
	{
	public:
        static void InitializeInstance(GLFWwindow* window);
	private:
		vk::Instance _instance;
	};


}