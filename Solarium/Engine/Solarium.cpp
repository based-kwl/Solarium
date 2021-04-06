// Solarium.cpp : Defines the entry point for the application.
//

#include "../Typedef.h"
#include "../Defines.hpp"
#include "Solarium.hpp"
#include "Engine.hpp"
using namespace std;

int main( int argc, const char** argv )
{
	Solarium::Logger::Log("eeeeeeeee");
	Solarium::Engine* engine = new Solarium::Engine("Sol", 1920, 1080);
	engine->Run();
	delete engine;
	return 0;
}
