#pragma once

#include "../Typedef.h"

namespace Solarium
{
	class Platform;

	class Engine
	{
	public:
		Engine(const char* applicationName);

		void Run();

		void OnLoop(const uint32_t deltaTime);
	private:
		Platform* _platform;
	};
}