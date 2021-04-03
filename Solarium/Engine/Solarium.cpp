// Solarium.cpp : Defines the entry point for the application.
//

#include "../Typedef.h"
#include "../Defines.h"
#include "Solarium.h"
#include <GLFW/glfw3.h>
#include "Logger.h"
using namespace std;

int main( int argc, const char** argv )
{
	Solarium::Logger::Log("Lukas is dum and has %d chromies", 50);
	return 0;
}
