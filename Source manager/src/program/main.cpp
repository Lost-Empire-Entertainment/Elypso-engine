//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include "glad.h"
#include "glfw3.h"

//program
#include "main.hpp"

#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <Windows.h>
#include <locale>
#include <codecvt>

using std::cout;
using std::endl;
using std::isspace;
using std::exception;
using std::system;
using std::filesystem::current_path;
using std::filesystem::exists;
using std::filesystem::status;
using std::filesystem::perms;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::file_status;
using std::filesystem::status;

using Core::Render;

int main()
{
	Render::GLFWSetup();
	Render::WindowSetup();
	Render::GladSetup();
	Render::GUISetup();

	Render::AddProtectedPaths();

	Render::WindowLoop();

	Render::Shutdown();

	return 0;
}

namespace Core
{
	void Render::GLFWSetup()
	{
		string output1 = programName;
		ConsoleWindow_WriteToConsole(output1, true);
		string output2 = "Copyright (C) Greenlaser 2023";
		ConsoleWindow_WriteToConsole(output2, true);

		ConsoleWindow_WriteToConsole("", true);

		ConsoleWindow_WriteToConsole("Initializing GLFW...");

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		ConsoleWindow_WriteToConsole("GLFW initialized successfully!");
	}

	void Render::WindowSetup()
	{
		ConsoleWindow_WriteToConsole("Creating window...");

		window = glfwCreateWindow(
			width,
			height,
			programName.c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			ConsoleWindow_WriteToConsole("Error: Failed to create window!");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetWindowSizeLimits(window, 1280, 720, 1280, 720);
		glfwSwapInterval(1);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		ConsoleWindow_WriteToConsole("Window created successfully!");
	}

	void Render::GladSetup()
	{
		ConsoleWindow_WriteToConsole("Initializing Glad...");

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ConsoleWindow_WriteToConsole("Error: Failed to initialize Glad!");
			return;
		}

		ConsoleWindow_WriteToConsole("Glad initialized successfully!");
	}

	void Render::GUISetup()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();

		string fontPath = current_path().generic_string() + "/files/fonts/coda/Coda-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f);

		string output = "Successfully loaded font from '" + fontPath + "'!";
		ConsoleWindow_WriteToConsole(output);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = fontScale;
	}

	void Render::AddProtectedPaths()
	{
		protectedPaths.push_back("C:\\$WinREAgent");
		protectedPaths.push_back("C:\\PerfLogs");
		protectedPaths.push_back("C:\\ProgramData");
		protectedPaths.push_back("C:\\Recovery");
		protectedPaths.push_back("C:\\Windows");
	}

	void Render::WindowLoop()
	{
		ConsoleWindow_WriteToConsole("");
		ConsoleWindow_WriteToConsole("Entering window loop...");

		while (!glfwWindowShouldClose(Render::window))
		{
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

			Window_Main();
			Window_Console();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void Render::Window_Main()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Main", NULL, windowFlags);

		MainWindow_HowToUse();
		MainWindow_HowToUse_HowToUse();
		MainWindow_Reconfigure_CMake();
		MainWindow_InstallEngine();
		MainWindow_CleanVS();
		MainWindow_CleanVS_Confirm();
		MainWindow_CleanEngine();
		MainWindow_CleanEngine_Confirm();
		MainWindow_InputField();

		ImGui::End();
	}
	void Render::MainWindow_HowToUse()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - buttonSize.x) * 0.5f,
			(windowSize.y - 400 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("How to use", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Start 'How to use'");
			isHowToUseOpen = true;
		}
	}
	void Render::MainWindow_HowToUse_HowToUse()
	{
		ImVec2 initialPos(400, 200);
		ImVec2 initialSize(600, 600);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		if (isHowToUseOpen
			&& ImGui::Begin("How to use", NULL, windowFlags))
		{
			ImGui::Text("Configure CMake:");
			ImGui::TextWrapped("Sets up engine installer, also required to run when source or header files are added or removed.");

			ImGui::Text("");

			ImGui::Text("Install engine:");
			ImGui::TextWrapped("Installs the engine to the chosen path. Leave blank if you want to install to build/Release in project folder.");

			ImGui::Text("");

			ImGui::Text("Clean engine:");
			ImGui::TextWrapped("Finds and deletes the engine executable parent folder and the engine documents folder.");

			ImGui::Text("");

			ImGui::Text("Clean Visual Studio:");
			ImGui::TextWrapped("Finds and deletes Visual Studio generated files.");

			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 buttonSize(120, 30);
			ImVec2 buttonPos(
				(windowSize.x - buttonSize.x) * 0.5f,
				(windowSize.y + 400 - buttonSize.y) * 0.5f);

			ImGui::SetCursorPos(buttonPos);

			if (ImGui::Button("Close", buttonSize))
			{
				ConsoleWindow_WriteToConsole("Close 'How to use'");
				isHowToUseOpen = false;
			}

			ImGui::End();
		}
	}
	void Render::MainWindow_Reconfigure_CMake()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - buttonSize.x) * 0.5f, 
			(windowSize.y - 150 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Reconfigure CMake", buttonSize))
		{
			string output;

			ConsoleWindow_WriteToConsole("Start 'Reconfigure CMake'");

			string currentFolder = current_path().stem().string();
			output = "Current folder name: " + currentFolder;
			ConsoleWindow_WriteToConsole(output);
			if (currentFolder != "build"
				&& currentFolder != "x64-debug"
				&& currentFolder != "x64-release")
			{
				ConsoleWindow_WriteToConsole("Error: Tried to run 'Reconfigure CMake' from an unknown path!", true);
				return;
			}

			//go to the Engine folder, assuming the structure is the same as the repository
			path engineFolderPath;

			if (currentFolder == "build")
			{
				engineFolderPath = current_path().parent_path().parent_path().parent_path() / "Engine";
			}
			else if (currentFolder == "x64-debug"
					 || currentFolder == "x64-release")
			{
				engineFolderPath = current_path().parent_path().parent_path().parent_path().parent_path() / "Engine";
			}

			output = "Attempting to find ' " + engineFolderPath.string() + " '...";
			ConsoleWindow_WriteToConsole(output, true);
			if (!exists(engineFolderPath))
			{
				ConsoleWindow_WriteToConsole("Error: Did not find engine folder! Did you reorganize the repository structure?", true);
				return;
			}

			ConsoleWindow_WriteToConsole("Found engine folder! Starting CMake configuration...", true);

			string buildBatPath = engineFolderPath.string() + "\\build.bat";
			output = "Attempting to find ' " + buildBatPath + " ' ...";
			ConsoleWindow_WriteToConsole(output, true);
			if (!exists(buildBatPath))
			{
				ConsoleWindow_WriteToConsole("Error: Did not find build.bat! Stopping CMake configuration.", true);
				return;
			}

			ConsoleWindow_WriteToConsole("Found build.bat!", true);

			ConsoleWindow_WriteToConsole("Attempting to run build.bat with command 'cmake'...", true);

			RunBatFile(buildBatPath, "cmake");
		}
	}
	void Render::MainWindow_InstallEngine()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - buttonSize.x) * 0.5f,
			(windowSize.y + 100 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Install engine", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Start 'Install engine'");
		}
	}
	void Render::MainWindow_CleanVS()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x + 225 - buttonSize.x) * 0.5f,
			(windowSize.y + 350 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Clean Visual Studio", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Start 'Clean Visual Studio'");
			isCleanVSConfirmOpen = true;
		}
	}
	void Render::MainWindow_CleanVS_Confirm()
	{
		ImVec2 initialPos(400, 200);
		ImVec2 initialSize(300, 300);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		if (isCleanVSConfirmOpen
			&& ImGui::Begin("Clean Visual Studio folders", NULL, windowFlags))
		{
			ImGui::Text("Are you sure you want to clean the Visual Studio folders?");

			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 buttonSize(120, 30);
			ImVec2 confirmButtonPos(
				(windowSize.x - 225 - buttonSize.x) * 0.5f,
				(windowSize.y + 225 - buttonSize.y) * 0.5f);

			ImGui::SetCursorPos(confirmButtonPos);

			if (ImGui::Button("Confirm", buttonSize))
			{
				ConsoleWindow_WriteToConsole("Confirm 'Clean Visual Studio'");
				isCleanVSConfirmOpen = false;
			}

			ImVec2 declineButtonPos(
				(windowSize.x + 225 - buttonSize.x) * 0.5f,
				(windowSize.y + 225 - buttonSize.y) * 0.5f);

			ImGui::SetCursorPos(declineButtonPos);

			if (ImGui::Button("Decline", buttonSize))
			{
				ConsoleWindow_WriteToConsole("Decline 'Clean Visual Studio'");
				isCleanVSConfirmOpen = false;
			}

			ImGui::End();
		}
	}
	void Render::MainWindow_CleanEngine()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - 225- buttonSize.x) * 0.5f,
			(windowSize.y + 350 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Clean engine", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Start 'Clean engine'");
			isCleanEngineConfirmOpen = true;
		}
	}
	void Render::MainWindow_CleanEngine_Confirm()
	{
		ImVec2 initialPos(400, 200);
		ImVec2 initialSize(300, 300);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		if (isCleanEngineConfirmOpen
			&& ImGui::Begin("Clean engine folders", NULL, windowFlags))
		{
			ImGui::Text("Are you sure you want to clean the engine folders?");

			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 buttonSize(120, 30);
			ImVec2 confirmButtonPos(
				(windowSize.x - 225 - buttonSize.x) * 0.5f,
				(windowSize.y + 225 - buttonSize.y) * 0.5f);

			ImGui::SetCursorPos(confirmButtonPos);

			if (ImGui::Button("Confirm", buttonSize))
			{
				ConsoleWindow_WriteToConsole("Confirm 'Clean engine'");
				isCleanEngineConfirmOpen = false;
			}

			ImVec2 declineButtonPos(
				(windowSize.x + 225 - buttonSize.x) * 0.5f,
				(windowSize.y + 225 - buttonSize.y) * 0.5f);

			ImGui::SetCursorPos(declineButtonPos);

			if (ImGui::Button("Decline", buttonSize))
			{
				ConsoleWindow_WriteToConsole("Decline 'Clean engine'");
				isCleanEngineConfirmOpen = false;
			}

			ImGui::End();
		}
	}
	void Render::MainWindow_InputField()
	{
		//move the input field to the bottom of the window
		ImGui::SetCursorPosY(
			ImGui::GetWindowHeight() - 
			ImGui::GetStyle().ItemSpacing.y -
			ImGui::GetFrameHeightWithSpacing());

		ImVec2 inputFieldWidth = ImGui::GetContentRegionAvail();

		ImGui::PushItemWidth(inputFieldWidth.x);

		ImGuiInputTextFlags inputFieldTextFlags =
			ImGuiInputTextFlags_EnterReturnsTrue;

		//press enter to insert install path
		if (ImGui::InputTextWithHint(
			"##inputfield",
			"Enter install path...",
			inputTextBuffer,
			sizeof(inputTextBuffer),
			inputFieldTextFlags,
			[](ImGuiInputTextCallbackData* data) -> int
			{
				//allow inserting text with Ctrl+V
				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)) &&
					ImGui::GetIO().KeyCtrl)
				{
					const char* clipboardText = glfwGetClipboardString(Render::window);
					if (clipboardText)
					{
						//insert clipboard text into the input buffer
						int textLength = strlen(clipboardText);
						int availableSpace = data->BufSize - data->BufTextLen;
						int copyLength = ImMin(textLength, availableSpace - 1); //leave space for null terminator
						if (copyLength > 0)
						{
							memcpy(data->Buf + data->CursorPos, clipboardText, copyLength);
							data->CursorPos += copyLength;
							data->BufTextLen += copyLength;
							data->Buf[data->CursorPos] = '\0'; 
						}
					}
				}
				return 0;
			}))
		{
			ConsoleWindow_ParseInput(inputTextBuffer);
			memset(inputTextBuffer, 0, sizeof(inputTextBuffer));
		}

		ImGui::PopItemWidth();
	}

	void Render::Window_Console()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Console", NULL, windowFlags);

		for (const auto& message : consoleMessages)
		{
			ImGui::TextWrapped("%s", message.c_str());
		}

		bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
		if (isNearBottom)
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::End();
	}
	void Render::ConsoleWindow_ParseInput(const string& message)
	{
		if (IsWhiteSpaceOrEmpty(message)) return;

		if (!IsPathAccessible(message)) return;

		string output = "'" + message + "' is a valid path!";
		ConsoleWindow_WriteToConsole(output, true);
	}
	void Render::ConsoleWindow_WriteToConsole(const string& message, bool printToConsole)
	{
		if (printToConsole) consoleMessages.push_back(message);
		cout << message << endl;

		//erases the first added message if a new one was added and count exceeded max count
		while (consoleMessages.size() > maxConsoleMessages)
		{
			consoleMessages.erase(consoleMessages.begin());
		}
	}

	bool Render::IsWhiteSpaceOrEmpty(const string& message)
	{
		for (char c : message)
		{
			if (!isspace(static_cast<unsigned char>(c)))
			{
				return false;
			}
		}
		return true;
	}
	bool Render::IsPathAccessible(const string& path)
	{
		string output;

		try
		{
			file_status fileStatus = status(path);

			if (!is_directory(path))
			{
				output = "Error: ' " + path + " ' is not a directory! Please insert a valid path.";
				ConsoleWindow_WriteToConsole(output, true);
				return false;
			}

			perms requiredPermissions = perms::owner_all;
			if ((fileStatus.permissions() & requiredPermissions) != requiredPermissions)
			{
				output = "Error: Insufficient permissions to install engine to ' " + path + " '! Please insert a path you have read, write and execution permissions for.";
				ConsoleWindow_WriteToConsole(output, true);
				return false;
			}

			if (path.find('/') != string::npos)
			{
				output = "Error: Invalid path format for ' " + path + " '! Please use backslashes, not forward slashes!";
				ConsoleWindow_WriteToConsole(output, true);
				return false;
			}

			if (ContainsProtectedPath(path))
			{
				output = "Error: ' " + path + " ' is an important system path and should not be written to! Please insert a valid path.";
				ConsoleWindow_WriteToConsole(output, true);
				return false;
			}

			return true;
		}
		catch (const exception& e)
		{
			output = "Error: " + string(e.what());
			ConsoleWindow_WriteToConsole(output, true);
			return false;
		}
	}
	bool Render::ContainsProtectedPath(const path& fullPath)
	{
		for (const auto& protectedPath : protectedPaths)
		{
			//check if fullPath is inside or equal to a protected path
			if (exists(protectedPath)
				&& fullPath.native().find(protectedPath.native()) == 0)
			{
				return true;
			}
		}
		return false;
	}

	void Render::RunBatFile(string filePath, string command)
	{
		string modifiedCommand = command + " 2>&1";

		SHELLEXECUTEINFO sei = { sizeof(sei) };

		sei.lpVerb = "runas"; //triggers elevation prompt
		sei.lpFile = filePath.c_str();
		sei.lpParameters = modifiedCommand.c_str();
		sei.nShow = SW_NORMAL;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;

		if (!ShellExecuteEx(&sei))
		{
			//MessageBox(NULL, "Error running script", "Error", MB_OK | MB_ICONERROR);

			string output = "Did not get permission from the user to run " + filePath + "!";
			ConsoleWindow_WriteToConsole(output, true);
		}
	}

	void Render::Shutdown()
	{
		//close any remaining open ImGui windows
		for (ImGuiWindow* window : ImGui::GetCurrentContext()->Windows)
		{
			if (window->WasActive)
			{
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::StyleColorsDark();
		ImGui::GetIO().IniFilename = nullptr;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();
	}
}