#pragma once

#define GFLW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "Logger.hpp"


struct GLFWwindow;

namespace Solarium
{

	class Engine;

	class Platform
	{
	public:
		Platform(const char* applicationName);
		~Platform();

		GLFWwindow* GetWindow() { return _window; }
		
		const bool StartGameLoop();

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		VkExtent2D getExtent() { return { static_cast<uint32_t>(1280), static_cast<uint32_t>(720) }; }

	private:
		GLFWwindow* _window;
	};
}