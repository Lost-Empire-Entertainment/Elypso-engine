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

using std::cout;
using std::endl;
using std::isspace;
using std::exception;
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

		MainWindow_Reconfigure_CMake();
		MainWindow_InstallEngine();
		MainWindow_InputField();

		ImGui::End();
	}
	void Render::MainWindow_Reconfigure_CMake()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - buttonSize.x) * 0.5f, 
			(windowSize.y - 100 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Reconfigure CMake", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Reconfigure CMake", true);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Needs to be ran every time an engine source or header file is added or removed.");
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
			ConsoleWindow_WriteToConsole("Install engine", true);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Install the engine to your desired location.");
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