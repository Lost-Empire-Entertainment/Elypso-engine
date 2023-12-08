//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "console.h"
#include "render.h"
#include "shutdown.h"

using namespace Core::Graphics;
using namespace Core::Console;
using Caller = ConsoleManager::Caller;
using Type = ConsoleManager::Type;

namespace Core
{
	void ShutdownManager::Shutdown()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::SHUTDOWN,
			Type::INFO,
			"Cleaning up resources...\n");

		Render::Shutdown();

		//de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &Render::VAO);
		glDeleteBuffers(1, &Render::VBO);
		glDeleteBuffers(1, &Render::EBO);

		//clean all glfw resources after program is closed
		glfwTerminate();

		ConsoleManager::WriteConsoleMessage(
			Caller::SHUTDOWN,
			Type::SUCCESS,
			"Shutdown complete!\n");
	}
}