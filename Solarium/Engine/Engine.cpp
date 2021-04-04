#include "Engine.hpp"
#include "Platform.hpp"
#include "VulkanRenderer.hpp"
#include "Logger.hpp"

namespace Solarium
{
	Engine::Engine(const char* applicationName)
	{
		Solarium::Logger::Log("INITIALIZING");
		_platform = new Platform(this, applicationName);
		auto renderer = new VulkanRenderer(_platform);
	}

	Engine::~Engine()
	{

	}

	void Engine::Run()
	{
		_platform->StartGameLoop();
	}

	void Engine::OnLoop(const uint32_t deltaTime)
	{

	}
}