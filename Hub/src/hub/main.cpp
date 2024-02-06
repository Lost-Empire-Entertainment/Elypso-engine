//external
#include "glad.h"
#include "stb_image.h"

//hub
#include "main.hpp"

#include <iostream>
#include <string>

using std::string;
using std::filesystem::current_path;

int main()
{
	Core::Initialize();

	while (!glfwWindowShouldClose(Core::window))
	{
		Core::Render();
	}

	Core::Shutdown();

	return 0;
}

void Core::Initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(
		SCR_WIDTH,
		SCR_HEIGHT,
		"Elypso hub 1.0.0",
		NULL,
		NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
	glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
	glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
	glfwSwapInterval(1);

	defaultPath = current_path().generic_string();

	int width, height, channels;
	string iconpath = defaultPath.string() + "/icon.png";
	unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

	GLFWimage icon{};
	icon.width = width;
	icon.height = height;
	icon.pixels = iconData;

	glfwSetWindowIcon(window, 1, &icon);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Core::UpdateAfterRescale(GLFWwindow* window, int width, int height)
{
	//Calculate the new aspect ratio
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	//Set the viewport based on the aspect ratio
	glViewport(0, 0, width, height);
}

void Core::Render()
{
	glClearColor(
		backgroundColor.x,
		backgroundColor.y,
		backgroundColor.z,
		1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//swap the front and back buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Core::Shutdown()
{
	glfwTerminate();
}