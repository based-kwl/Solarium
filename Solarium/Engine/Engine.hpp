#pragma once

#include "../Typedef.h"
#include "Device.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"

#include <stdexcept>
#include <memory>

namespace Solarium
{

	class Platform;
	class Engine
	{
	public:
		Engine(const char* applicationName, uint32_t width, uint32_t height);
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Run();

		void OnLoop(const uint32_t deltaTime);

		bool getFramebufferResized() { return framebufferResized; }
		void setFramebufferResized(bool resized) { framebufferResized = resized; }
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void cleanupSwapChain();

		void cleanup();

		//void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		Platform* _platform;
		Device* device;
		SwapChain* swapChain;
		Pipeline* pipeline;
		vk::PipelineLayout pipelineLayout;
		std::vector<vk::CommandBuffer> commandBuffers;

		bool framebufferResized = false;
	};
}