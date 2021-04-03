#include "Engine.h"
#include "Platform.h"

namespace Solarium
{
	Engine::Engine(const char* applicationName)
	{
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