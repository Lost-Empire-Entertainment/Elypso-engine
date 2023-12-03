//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "console.h"
#include "shader.h"
#include "shutdown.h"

void ShutdownManager::Shutdown(ShaderManager& shaderManager)
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::SHUTDOWN, 
		ConsoleManager::Type::INFO, 
		"Cleaning up resources...\n");

	//de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &shaderManager.VAO);
	glDeleteBuffers(1, &shaderManager.VBO);
	glDeleteProgram(shaderManager.shaderProgram);

	//clean all glfw resources after program is closed
	glfwTerminate();

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::SHUTDOWN, 
		ConsoleManager::Type::SUCCESS, 
		"Shutdown complete!\n");
}