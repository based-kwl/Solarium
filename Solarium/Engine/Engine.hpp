#pragma once

#include "../Typedef.h"
#include "Device.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Pipeline.hpp"
#include "VulkanRenderer.hpp"

namespace Solarium
{
	class Platform;
	class Engine
	{
	public:
		Engine(const char* applicationName);
		~Engine();
		void Run();

		void OnLoop(const uint32_t deltaTime);
	private:

		Platform* _platform;
		Device* device;
		Pipeline Pipeline{*device, "../../../Shaders/out/Test_shader.vert.spv", "../../../Shaders/out/Test_shader.frag.spv", Pipeline::defaultPipelineConfigInfo(1280, 720)};
	};
}