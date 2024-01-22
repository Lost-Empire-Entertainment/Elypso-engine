//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#include <Windows.h>

using std::cout;
using std::endl;
using std::filesystem::current_path;
using std::filesystem::exists;

using Core::Render;

int main()
{
	Render::GLFWSetup();
	Render::WindowSetup();
	Render::GladSetup();
	Render::GUISetup();

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
			"Source manager",
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
		MainWindow_BuildEngine();
		MainWindow_CleanVS();
		MainWindow_CleanVS_Confirm();
		MainWindow_CleanEngine();
		MainWindow_CleanEngine_Confirm();

		ImGui::End();
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
			ConsoleWindow_WriteToConsole("Start 'Reconfigure CMake'");
			ConsoleWindow_WriteToConsole("", true);

			RunBatFile("cmake");
		}
	}
	void Render::MainWindow_BuildEngine()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 buttonSize(200, 60);
		ImVec2 buttonPos(
			(windowSize.x - buttonSize.x) * 0.5f,
			(windowSize.y + 100 - buttonSize.y) * 0.5f);

		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Build engine", buttonSize))
		{
			ConsoleWindow_WriteToConsole("Start 'Build engine'");
			ConsoleWindow_WriteToConsole("", true);

			RunBatFile("build");
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
				ConsoleWindow_WriteToConsole("", true);

				RunBatFile("cleanvs");
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
				ConsoleWindow_WriteToConsole("", true);

				RunBatFile("cleanen");
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

	void Render::RunBatFile(string command)
	{
		string currentFolder = current_path().stem().string();
		string output = "Current folder name: " + currentFolder;
		ConsoleWindow_WriteToConsole(output);
		if (currentFolder != "Release"
			&& currentFolder != "x64-debug"
			&& currentFolder != "x64-release")
		{
			ConsoleWindow_WriteToConsole("Error: Tried to run build.bat from an unknown path! Did you reorganize the repository structure?", true);
			return;
		}

		//find build.bat, assuming the structure is the same as the repository
		path batPath;
		if (currentFolder == "Release")
		{
			batPath = current_path().parent_path().parent_path().parent_path() / "Engine\\build.bat";
		}
		else if (currentFolder == "x64-debug"
			     || currentFolder == "x64-release")
		{
			batPath = current_path().parent_path().parent_path().parent_path().parent_path() / "Engine\\build.bat" ;
		}


		output = "Attempting to find ' " + batPath.string() + " '...";
		ConsoleWindow_WriteToConsole(output);
		if (!exists(batPath))
		{
			ConsoleWindow_WriteToConsole("Error: Did not find build.bat! Did you reorganize the repository structure?", true);
			return;
		}

		output = "Found build.bat! Starting '" + command + "'...";
		ConsoleWindow_WriteToConsole(output, true);

		string batPathString = batPath.string();

		SHELLEXECUTEINFO sei = { sizeof(sei) };

		sei.lpVerb = "runas"; //triggers elevation prompt
		sei.lpFile = batPathString.c_str(); //path to build.bat
		sei.lpParameters = command.c_str(); //the build.bat command that is run
		sei.nShow = SW_NORMAL;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;

		if (!ShellExecuteEx(&sei))
		{
			string output = "Error: Did not get permission from the user to run build.bat!";
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