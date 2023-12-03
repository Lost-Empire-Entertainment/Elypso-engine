//external
#include "glad.h"

//engine
#include "console.h"
#include "render.h"
#include "shutdown.h"

ShaderManager RenderManager::shader(ShaderManager::vertexShader, ShaderManager::fragmentShader);

int RenderManager::WindowSetup()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::GLFW, 
		ConsoleManager::Type::INFO, 
		"Initializing GLFW...\n");

	if (glfwInit() != GLFW_TRUE) 
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::GLFW,
			ConsoleManager::Type::ERROR,
			"Failed to initialize GLFW!\n\n");
		ShutdownManager::Shutdown();
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::GLFW,
		ConsoleManager::Type::SUCCESS, 
		"GLFW initialized successfully!\n\n");

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::WINDOW_SETUP,
		ConsoleManager::Type::INFO, 
		"Creating window...\n");

	//create a window object holding all the windowing data
	RenderManager::window = glfwCreateWindow(
		RenderManager::SCR_WIDTH, 
		RenderManager::SCR_HEIGHT, 
		"Elypso engine", 
		NULL, 
		NULL);

	if (RenderManager::window == NULL)
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::GLFW,
			ConsoleManager::Type::ERROR,
			"Failed to create GLFW window!\n\n");

		ShutdownManager::Shutdown();
		return -1;
	}
	glfwMakeContextCurrent(RenderManager::window);
	glfwSetFramebufferSizeCallback(
		RenderManager::window, 
		RenderManager::UpdateAfterRescale);

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::WINDOW_SETUP,
		ConsoleManager::Type::SUCCESS, 
		"Window initialized successfully!\n\n");

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::GLAD,
		ConsoleManager::Type::INFO, 
		"Initializing GLAD...\n");

	//check if glad is initialized before continuing
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::GLAD,
			ConsoleManager::Type::ERROR, 
			"Failed to initialize GLAD!\n\n");
		return -1;
		ShutdownManager::Shutdown();
	}

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::GLAD,
		ConsoleManager::Type::SUCCESS, 
		"GLAD initialized successfully!\n\n");

	return 0;
}

//make sure the viewport matches the new window dimentions
void RenderManager::UpdateAfterRescale(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//this is run while the window is open
void RenderManager::WindowLoop()
{
	//clear the background to dark green
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//clear color
	glClear(GL_COLOR_BUFFER_BIT);

	//render the triangle
	RenderManager::shader.Use();
	glBindVertexArray(ShaderManager::VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//swap the front and back buffers
	glfwSwapBuffers(RenderManager::window);
	//poll for events and process them
	glfwPollEvents();
}