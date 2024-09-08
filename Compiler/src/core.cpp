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

using Graphics::Render;
using Utils::File;
using Utils::String;
using Graphics::GUI;

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

	void Compiler::CreateErrorPopup(const string& errorMessage)
	{
		string title = "Compiler has shut down";

		int result = MessageBoxA(nullptr, errorMessage.c_str(), title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) MainShutdown();
	}

	void Compiler::MainLoop()
	{
		cout << "Running Compiler...\n";

		while (!glfwWindowShouldClose(Render::window))
		{
			Render::RenderLoop();
		}
	}

	void Compiler::MainShutdown()
	{
		cout << "Shutting down Compiler...\n";

		GUI::GUIShutdown();
		glfwTerminate();
		ExitProcess(1);
	}
}