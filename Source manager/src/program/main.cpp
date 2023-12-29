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

#include <filesystem>

using std::cout;
using std::endl;
using std::filesystem::current_path;

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
		cout << "Initializing GLFW..." << endl;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "GLFW initialized successfully!" << endl;
	}

	void Render::WindowSetup()
	{
		cout << "Creating window..." << endl;

		window = glfwCreateWindow(
			width,
			height,
			programName.c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			cout << "Error: Failed to created GLFW window!" << endl;
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetWindowSizeLimits(window, 1280, 720, 1280, 720);
		glfwSwapInterval(1);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		//glfwSetScrollCallback(window, Input::ScrollCallback);
		//glfwSetKeyCallback(window, Input::KeyCallback);

		cout << "Window initialized successfully!" << endl;
	}

	void Render::GladSetup()
	{
		cout << "Initializing glad..." << endl;

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Error: Failed to initialize GLAD!" << endl;
			return;
		}

		cout << "GLAD initialized successfully!" << endl;
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

		cout << "Successfully loaded font from '" << fontPath << "'!" << endl;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = fontScale;
	}

	void Render::WindowLoop()
	{
		cout << "\nEntering window loop..." << endl;

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
			cout << "Reconfigure CMake" << endl;
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
			cout << "Install engine" << endl;
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
			ConsoleWindow_WriteToConsole(inputTextBuffer);
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
	void Render::ConsoleWindow_WriteToConsole(const string& message)
	{
		consoleMessages.push_back(message);

		//erases the first added message if a new one was added and count exceeded max count
		while (consoleMessages.size() > maxConsoleMessages)
		{
			consoleMessages.erase(consoleMessages.begin());
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