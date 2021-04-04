#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Solarium
{

	static class VulkanHelper
	{
	public:
        vk::Instance initializeInstance(GLFWwindow* window);
	private:
        vk::Instance _instance;
	};


}