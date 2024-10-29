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

#include "core.hpp"
#include "render.hpp"
#include "gui.hpp"

using Graphics::GUI::GUI_Hub;
using Graphics::Render;

using std::cout;
using std::exception;
using std::filesystem::current_path;
using std::filesystem::exists;
using std::ofstream;
using std::ifstream;
using std::wstring;
using std::getline;
using std::istringstream;
using std::to_string;

namespace Core
{
	void Hub::Initialize()
	{
		if (IsThisProcessAlreadyRunning("Elypso hub.exe"))
		{
			CreateErrorPopup("Elypso Hub is already running.");
		}

		cout << "\n==================================================\n"
			<< "\n"
			<< "ENTERED ELYPSO HUB\n"
			<< "\n"
			<< "==================================================\n"
			<< ".\n"
			<< ".\n"
			<< ".\n\n";

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
			docsPath = narrowPath + "/Elypso hub";
		}

		if (!exists(docsPath)) create_directory(docsPath);

		configFilePath = docsPath.string() + "/config.txt";
		if (!exists(configFilePath))
		{
			ofstream configFile(configFilePath);
			if (!configFile.is_open())
			{
				CreateErrorPopup("Failed to create config file.");
			}
			configFile.close();
		}
		LoadConfigFile();

		Render::Initialize();
	}

	void Hub::LoadConfigFile()
	{
		string line;
		ifstream configFile(configFilePath);
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
				vector<string> splitLine = StringSplit(line, '=');
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
					vector<string> resolution = StringSplit(value, ',');
					Render::currentWidth = stoul(resolution.at(0));
					Render::currentHeight = stoul(resolution.at(1));
					cout << "Set resolution to '" << to_string(Render::currentWidth) << "x" << to_string(Render::currentHeight) << "'!\n";
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

	void Hub::SaveConfigFile()
	{
		if (exists(configFilePath)) remove(configFilePath);

		ofstream configFile(configFilePath);

		configFile << "resolution= " << to_string(Render::currentWidth) << ", " << to_string(Render::currentHeight) << "\n";
		cout << "Set resolution to '" << to_string(Render::currentWidth) << "x" << to_string(Render::currentHeight) << "'!\n";

		string enginePathResult = enginePath.string() != "" ? enginePath.string() : "";
		configFile << "engine path= " << enginePathResult << "\n";
		cout << "Set engine path to '" << enginePath.string() << "'!\n";

		string projectsFolderResult = projectsFolderPath.string() != "" ? projectsFolderPath.string() : "";
		configFile << "projects folder= " << projectsFolderResult << "\n";
		cout << "Set projects folder to '" << projectsFolderPath.string() << "'!\n";

		configFile.close();
	}

	vector<string> Hub::StringSplit(const string& input, char delimiter)
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

	bool Hub::IsThisProcessAlreadyRunning(const string& processName)
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
			if (strcmp(pe32.szExeFile, processName.c_str()) == 0)
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

	void Hub::CreateErrorPopup(const char* errorMessage)
	{
		string title = "Elypso Hub has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
			<< "HUB SHUTDOWN"
			<< "\n\n"
			<< errorMessage
			<< "\n"
			<< "===================="
			<< "\n";

		int result = MessageBoxA(nullptr, errorMessage, title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown();
	}

	//reset last idle activity timer
	static double lastActivityTime = 0.0f;
	void Hub::UpdateActivityTime()
	{
		lastActivityTime = glfwGetTime();
	}
	//check if any input has occured within the idle time
	bool Hub::IsInputActive()
	{
		const double idleThreshold = 1.0;
		double currentTime = glfwGetTime();
		double idleTime = currentTime - lastActivityTime;

		bool inputActive = idleTime <= idleThreshold;
		return inputActive;
	}

	//counts as idle if minimized
	//or unfocused and not compiling
	//or if focused and no input was detected and not compiling
	bool Hub::IsUserIdle()
	{
		//checks if window is minimized
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		if (width == 0 || height == 0) return true;

		//checks if window is unfocused and user is not compiling
		if (glfwGetWindowAttrib(Render::window, GLFW_FOCUSED) == GLFW_FALSE)
		{
			return true;
		}

		//checks if not compiling and no input is detected
		return !IsInputActive();
	}

	void Hub::Render()
	{
		cout << "Successfully reached render loop!\n\n";
		cout << "==================================================\n\n";

		isHubRunning = true;

		while (isHubRunning)
		{
			Render::Run();
			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isHubRunning = false;
			}
		}
	}

	void Hub::Shutdown()
	{
		cout << "Shutting down Elypso Hub...\n";

		isHubRunning = false;

		SaveConfigFile();
		GUI_Hub::Shutdown();
		glfwTerminate();
	}
}