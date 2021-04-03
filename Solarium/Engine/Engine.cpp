#include "Engine.h"
#include "Platform.h"
#include "Logger.h"

namespace Solarium
{
	Engine::Engine(const char* applicationName)
	{
		Solarium::Logger::Log("INITIALIZING");
		_platform = new Platform(this, applicationName);
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