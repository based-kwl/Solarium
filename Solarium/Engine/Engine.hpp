#pragma once

#include "../Typedef.h"
#include "Device.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Pipeline.hpp"
#include "VulkanRenderer.hpp"
#include "SwapChain.hpp"

#include <stdexcept>
#include <memory>

namespace Solarium
{
	class Platform;
	class Engine
	{
	public:
		Engine(const char* applicationName);
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Run();

		void OnLoop(const uint32_t deltaTime);
	private:

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		Platform* _platform;
		Device* device;
		SwapChain* swapChain;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<vk::CommandBuffer> commandBuffers;
	};
}