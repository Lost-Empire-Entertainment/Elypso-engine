//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <filesystem>

#include "core.hpp"
#include "render.hpp"
#include "configFile.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "gui.hpp"
#include "configFile.hpp"
#include "compile.hpp"

using Graphics::Render;
using Utils::File;
using Utils::String;
using Graphics::GUI;
using Core::ConfigFile;
using Core::TheCompiler;

using std::cout;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::current_path;

namespace Core
{
	void Compiler::MainInitialize()
	{
		if (IsThisProcessAlreadyRunning("Compiler.exe"))
		{
			CreateErrorPopup("Compiler is already running!");
		}

		cout << "Copyright (C) Lost Empire Entertainment 2024\n\n";

		cout << "Initializing Compiler...\n";

		//
		// SET DOCUMENTS PATH
		//

		PWSTR docsFolderWidePath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docsFolderWidePath);
		if (SUCCEEDED(result))
		{
			wstring wPath(docsFolderWidePath);
			CoTaskMemFree(docsFolderWidePath); //free the allocated memory

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

			docsPath = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\Compiler";

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);

			cout << "Compiler documents path: " << docsPath << "\n";

			filesPath = current_path().generic_string() + "\\files";
			filesPath = String::CharReplace(filesPath, '/', '\\');
			if (!exists(filesPath))
			{
				CreateErrorPopup("Couldn't find files folder!");
				return;
			}

			cout << "Files path: " << filesPath << "\n";

			configFilePath = docsPath + "\\config.txt";

			ConfigFile::LoadData();
		}
		else
		{
			CreateErrorPopup("Couldn't find engine documents folder!");
		}

		Render::RenderInitialize();
	}

	bool Compiler::IsThisProcessAlreadyRunning(const string& processName)
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

	void Compiler::CreateErrorPopup(const string& errorMessage)
	{
		string title = "Compiler has shut down";

		int result = MessageBoxA(nullptr, errorMessage.c_str(), title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) MainShutdown();
	}

	//reset last idle activity timer
	static double lastActivityTime = 0.0f;
	void Compiler::UpdateActivityTime()
	{
		lastActivityTime = glfwGetTime();
	}
	//check if any input has occured within the idle time
	bool Compiler::IsInputActive()
	{
		const double idleThreshold = 1.0;
		double currentTime = glfwGetTime();
		double idleTime = currentTime - lastActivityTime;

		bool inputActive = idleTime <= idleThreshold;
		return inputActive;
	}

	//reset last end of compile timer
	static double lastEndOfCompileOnceTime = 0.0;
	static bool calledEndOfCompileOnce = true;
	void Compiler::UpdateLastEndOfCompilerOnceTime()
	{
		lastEndOfCompileOnceTime = glfwGetTime();
		TheCompiler::isCompiling = false;
	}
	//wait for a short period before setting compilation as done 
	//to let the compiler ui reach its end correctly
	void Compiler::WaitBeforeCompileEnd()
	{
		const double endThreshold = 1.0;
		double currentTime = glfwGetTime();
		double endTime = currentTime - lastEndOfCompileOnceTime;

		bool endOfTime = endTime >= endThreshold;
		if (endOfTime) calledEndOfCompileOnce = true;
	}

	//returns true if short timer after compile hasnt ended or if still compiling,
	//returns false if short timer after compile is over
	bool Compiler::IsCompiling(const bool& isCompiling)
	{
		if (isCompiling)
		{
			calledEndOfCompileOnce = false;
			return true;
		}
		else
		{
			if (!calledEndOfCompileOnce) WaitBeforeCompileEnd();
			return !calledEndOfCompileOnce;
		}
	}

	//counts as idle if minimized
	//or unfocused and not compiling
	//or if focused and no input was detected and not compiling
	bool Compiler::IsUserIdle()
	{
		//checks if window is minimized
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		if (width == 0 || height == 0) return true;

		//checks if window is unfocused and user is not compiling
		if (glfwGetWindowAttrib(Render::window, GLFW_FOCUSED) == GLFW_FALSE)
		{
			return !IsCompiling(TheCompiler::isCompiling);
		}

		//checks if not compiling and no input is detected
		return !IsCompiling(TheCompiler::isCompiling)
			&& !IsInputActive();
	}

	void Compiler::MainLoop()
	{
		cout << "Running Compiler...\n";

		isCompilerRunning = true;

		while (isCompilerRunning)
		{
			Render::RenderLoop();

			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isCompilerRunning = false;
			}
		}
	}

	void Compiler::MainShutdown()
	{
		cout << "Shutting down Compiler...\n";

		isCompilerRunning = false;
		
		ConfigFile::SaveData();
		GUI::GUIShutdown();
		glfwTerminate();
	}
}