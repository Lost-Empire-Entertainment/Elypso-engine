//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <fstream>
#include <filesystem>

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
using std::ofstream;
using std::ifstream;
using std::wstring;
using std::getline;
using std::istringstream;
using std::to_string;

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
	cout << "Initializing Elypso Hub...\n";

	PWSTR path;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
	if (SUCCEEDED(result))
	{
		wstring wPath(path);
		CoTaskMemFree(path); //free the allocated memory

		//get the required buffer size
		int size_needed = WideCharToMultiByte(
			CP_UTF8,
			0,
			wPath.c_str(),
			static_cast<int>(wPath.length()),
			NULL,
			0,
			NULL,
			NULL);

		//convert wide string to utf-8 encoded narrow string
		string narrowPath(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			wPath.c_str(),
			static_cast<int>(wPath.length()),
			&narrowPath[0],
			size_needed,
			NULL,
			NULL);

		size_t pos = 0;
		string incorrectSlash = "\\";
		string correctSlash = "/";
		while ((pos = narrowPath.find(incorrectSlash, pos)) != string::npos)
		{
			narrowPath.replace(pos, incorrectSlash.length(), correctSlash);
			pos += correctSlash.length();
		}
		Core::docsPath = narrowPath + "/Elypso hub";
	}

	if (!exists(Core::docsPath)) create_directory(Core::docsPath);

	Core::configFilePath = Core::docsPath.string() + "/config.txt";
	if (!exists(Core::configFilePath))
	{
		ofstream configFile(Core::configFilePath);
		if (!configFile.is_open())
		{
			CreateErrorPopup(
				"Initialization failed", 
				"Fauled to create config file!");
		}
		configFile.close();
	}
	Core::LoadConfigFile();

	cout << "Initializing GLFW...\n";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	cout << "Initializing window...\n";

	if (currentWidth == 0) currentWidth = 1280;
	if (currentHeight == 0) currentHeight = 720;
	window = glfwCreateWindow(
		currentWidth,
		currentHeight,
		"Elypso hub 1.0.0",
		NULL,
		NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
	glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
	glfwSetWindowSizeLimits(window, 600, 300, 7680, 4320);
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

	UpdateAfterRescale(window, currentWidth, currentHeight);

	GUI::Initialize();
}

void Core::LoadConfigFile()
{
	string line;
	ifstream configFile (configFilePath);
	if (!configFile.is_open())
	{
		cout << "Error: Failed to open config file!\n";
		return;
	}

	while (getline(configFile, line))
	{
		if (!line.empty()
			&& line.find("=") != string::npos)
		{
			vector<string> splitLine = Core::StringSplit(line, '=');
			string type = splitLine[0];
			string value = splitLine[1];

			//remove one space in front of value if it exists
			if (value[0] == ' ') value.erase(0, 1);
			//remove one space in front of each value comma if it exists
			for (size_t i = 0; i < value.length(); i++)
			{
				if (value[i] == ','
					&& i + 1 < value.length()
					&& value[i + 1] == ' ')
				{
					value.erase(i + 1, 1);
				}
			}

			if (type == "resolution")
			{
				vector<string> resolution = Core::StringSplit(value, ',');
				currentWidth = stoul(resolution.at(0));
				currentHeight = stoul(resolution.at(1));
				cout << "Set resolution to '" << to_string(currentWidth) << "x" << to_string(currentHeight) << "'!\n";
			}

			if (type == "engine path")
			{
				enginePath = value;
				cout << "Set engine path to '" << enginePath.string() << "'!\n";
			}

			if (type == "projects folder")
			{
				projectsFolderPath = value;
				cout << "Set projects folder to '" << projectsFolderPath.string() << "'!\n";
			}
		}
	}
	configFile.close();

	cout << "Successfully loaded config file!\n";
}

void Core::SaveConfigFile()
{
	if (exists(configFilePath)) remove(configFilePath);

	ofstream configFile(configFilePath);

	configFile << "resolution= " << to_string(currentWidth) << ", " << to_string(currentHeight) << "\n";
	cout << "Set resolution to '" << to_string(currentWidth) << "x" << to_string(currentHeight) << "'!\n";

	string enginePathResult = enginePath.string() != "" ? enginePath.string() : "";
	configFile << "engine path= " << enginePathResult << "\n";
	cout << "Set engine path to '" << enginePath.string() << "'!\n";

	string projectsFolderResult = projectsFolderPath.string() != "" ? projectsFolderPath.string() : "";
	configFile << "projects folder= " << projectsFolderResult << "\n";
	cout << "Set projects folder to '" << projectsFolderPath.string() << "'!\n";

	configFile.close();
}

vector<string> Core::StringSplit(const string& input, char delimiter)
{
	vector<string> tokens;
	string token;
	istringstream tokenStream(input);
	while (getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void Core::UpdateAfterRescale(GLFWwindow* window, int width, int height)
{
	//Calculate the new aspect ratio
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	//Set the viewport based on the aspect ratio
	glViewport(0, 0, width, height);

	currentWidth = width;
	currentHeight = height;
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
	SaveConfigFile();

	GUI::Shutdown();

	glfwTerminate();
}