
#include "Platform.hpp"

namespace Solarium
{
	Platform::Platform(const char* applicationName, uint32_t width, uint32_t height)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		this->width = width;
		this->height = height;
		_window = glfwCreateWindow(width, height, applicationName, nullptr, nullptr);
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

	void Platform::createWindowSurface(vk::Instance instance, vk::SurfaceKHR* surface)
	{
		VkSurfaceKHR tmpSurface;
		if (glfwCreateWindowSurface(VkInstance(instance), _window, nullptr, &tmpSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface.");
		}
		*surface = vk::SurfaceKHR(tmpSurface);
	}
}