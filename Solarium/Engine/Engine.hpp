#pragma once

#include "../Typedef.hpp"

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
	};
}