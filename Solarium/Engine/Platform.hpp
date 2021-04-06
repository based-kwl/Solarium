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
		Platform(const char* applicationName, uint32_t width, uint32_t height);
		~Platform();

		GLFWwindow* GetWindow() { return _window; }
		
		const bool StartGameLoop();

		void createWindowSurface(vk::Instance instance, vk::SurfaceKHR* surface);

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

	private:
		GLFWwindow* _window;
		uint32_t width;
		uint32_t height;
	};
}