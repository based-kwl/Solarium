#prgma once

struct GLFWwindow;

namespace Solarium
{
	class Engine;
	class Platform
	{
	public:
		Platform(Engine* engine, const char* applicationName);
		~Platform();

		GLFWwindow* GetWindow() { return _window; }
		
		const bool StartGameLoop();

	private:
		GLFWwindow* _window;
	};
}