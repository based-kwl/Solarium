
#include "Platform.hpp"

namespace Solarium
{
	Platform::Platform(const char* applicationName)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		_window = glfwCreateWindow(1280, 720, applicationName, nullptr, nullptr);
		glfwSetWindowUserPointer(_window, this);
		Logger::Log("Window Created.");
	}
	
	Platform:: ~Platform()
	{
		if (_window)
		{
			glfwDestroyWindow(_window);
		}
		glfwTerminate();
	}

	const bool Platform::StartGameLoop()
	{
		return true;
	}

	void Platform::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface.");
		}
	}
}