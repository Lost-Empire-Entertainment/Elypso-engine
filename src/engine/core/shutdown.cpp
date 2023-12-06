//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "console.h"
#include "shader.h"
#include "shutdown.h"

namespace Core
{
	void ShutdownManager::Shutdown()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::SHUTDOWN,
			Core::Console::ConsoleManager::Type::INFO,
			"Cleaning up resources...\n");

		//de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &Graphics::ShaderManager::VAO);
		glDeleteBuffers(1, &Graphics::ShaderManager::VBO);
		glDeleteProgram(Graphics::ShaderManager::shaderProgram);

		//clean all glfw resources after program is closed
		glfwTerminate();

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::SHUTDOWN,
			Core::Console::ConsoleManager::Type::SUCCESS,
			"Shutdown complete!\n");
	}
}