
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

	void Platform::createWindowSurface(vk::Instance instance, vk::SurfaceKHR* surface)
	{
		VkSurfaceKHR tmpSurface;
		VkResult thing = glfwCreateWindowSurface(VkInstance(instance), _window, nullptr, &tmpSurface);
		if (thing != VK_SUCCESS)
		{
			const char* des;
			int code = glfwGetError(&des);
			Logger::Error(des);
			throw std::runtime_error("Failed to create window surface.");
		}
		*surface = vk::SurfaceKHR(tmpSurface);
	}
}