//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>

//external
#include "glad.h"
#include "stb_image.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "main.hpp"
#include "gui.hpp"

using std::cout;
using std::exception;
using std::filesystem::current_path;

int main()
{
	if (Core::IsThisProcessAlreadyRunning("Elypso hub.exe"))
	{
		string title = "Engine already running";
		string message = "Error: 'Elypso hub' is already running!";

		Core::CreateErrorPopup(title.c_str(), message.c_str());
	}

	cout << "\n==================================================\n"
		<< "\n"
		<< "ENTERED ELYPSO HUB\n"
		<< "\n"
		<< "==================================================\n"
		<< ".\n"
		<< ".\n"
		<< ".\n\n";

	Core::Initialize();

	//get all files if any new ones were added
	GUI::UpdateFileList();

	//assign engine path from config file
	GUI::SetEnginePathFromConfigFile();

	cout << "Successfully reached render loop!\n\n";
	cout << "==================================================\n\n";

	while (!glfwWindowShouldClose(Core::window))
	{
		Core::Render();
	}

	Core::Shutdown();

	return 0;
}

//
// CORE
//

void Core::Initialize()
{
	cout << "Initializing GLFW...\n";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	cout << "Initializing window...\n";

	window = glfwCreateWindow(
		SCR_WIDTH,
		SCR_HEIGHT,
		"Elypso hub 1.0.0",
		NULL,
		NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
	glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
	glfwSetWindowSizeLimits(window, 800, 500, 7680, 4320);
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

	cout << "Initializing GLAD...\n";

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);

	GUI::Initialize();
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

	GUI::Render();

	//swap the front and back buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Core::CreateErrorPopup(const char* errorTitle, const char* errorMessage)
{
	int result = MessageBoxA(nullptr, errorMessage, errorTitle, MB_ICONERROR | MB_OK);

	if (result == IDOK) Shutdown(true);
}


bool Core::IsThisProcessAlreadyRunning(const string& processName)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32{};
	bool processFound = false;
	DWORD currentProcessId = GetCurrentProcessId();

	//take a snapshot of all processes
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout << "Error: CreateToolhelp32Snapshot failed!\n";
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "Error: Process32First failed!\n";
		CloseHandle(hProcessSnap);
		return false;
	}

	do
	{
		//compare the current process name with the one to check
		if (_stricmp(pe32.szExeFile, processName.c_str()) == 0)
		{
			//check if this is not the current process
			if (pe32.th32ProcessID != currentProcessId)
			{
				processFound = true;
				break;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return processFound;
}

void Core::Shutdown(bool immediate)
{
	GUI::Shutdown();

	glfwTerminate();
}