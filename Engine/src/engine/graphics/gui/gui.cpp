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
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_debugmenu.hpp"
#include "gui_inspector.hpp"
#include "gui_projecthierarchy.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "searchUtils.hpp"
#include "fileUtils.hpp"
#include "browserUtils.hpp"
#include "shutdown.hpp"
#include "gameobject.hpp"

#include <string>
#include <filesystem>

using std::cout;
using std::endl;
using std::stoi;
using std::to_string;
using std::exception;
using std::filesystem::path;
using std::filesystem::exists;
using glm::clamp;
using glm::vec3;

using Core::Engine;
using Core::Input;
using Core::ConsoleManager;
using Core::ShutdownManager;
using Utils::Search;
using Utils::File;
using Utils::Browser;
using Utils::String;
using Graphics::GUI::GUIConsole;
using Graphics::GUI::GUIDebugMenu;
using Graphics::GUI::GUIInspector;
using Graphics::GUI::GUIProjectHierarchy;
using Core::ECS::GameObject;
using Core::ECS::Transform;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

	EngineGUI& EngineGUI::GetInstance()
	{
		static EngineGUI instance;
		return instance;
	}

	void EngineGUI::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Engine::docsPath + "/imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::enginePath + "/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		CustomizeImGuiStyle();
	}

	void EngineGUI::CustomizeImGuiStyle()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = fontScale;
	}

	int EngineGUI::GetScreenWidth()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return width;
	}

	int EngineGUI::GetScreenHeight()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return height;
	}

	float EngineGUI::GetScreenRefreshRate()
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		return static_cast<float>(videoMode->refreshRate);
	}

	void EngineGUI::Render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		RenderTopBar();

		GUIConsole::RenderConsole();
		GUIDebugMenu::RenderDebugMenu();
		GUIInspector::RenderInspector();
		GUIProjectHierarchy::RenderProjectHierarchy(Engine::filesPath);
		if (GUIProjectHierarchy::rootPath.empty())
		{
			GUIProjectHierarchy::rootPath = Engine::filesPath;
		}

		RenderVersionCheckWindow();

		if (Input::inputSettings.printIMGUIToConsole)
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI, 
					Type::DEBUG, 
					"Left mouse button clicked!\n");
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EngineGUI::RenderTopBar()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Open is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Save"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Save is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Save As"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Save As is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("New Project"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"New Project is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Exit"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"User closed engine exit button.\n");
				ShutdownManager::shouldShutDown = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(50 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Undo is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Redo"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Redo is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Cut"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Cut is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Copy"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Copy is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Paste"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Paste is a placeholder button and does not yet have any functions.");
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(100 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Asset"))
		{
			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					GameObject obj = GameObject::CreateCube(
						vec3(0.0f, 0.0f, 0.0f),
						vec3(1.0f, 1.0f, 1.0f),
						vec3(1.0f, 0.0f, 0.0f),
						32.0f);

					unsigned int ID = obj.GetID();
					vec3 pos = obj.GetComponent<Transform>()->position;
					string posX = to_string(pos.x);
					string posY = to_string(pos.y);
					string posZ = to_string(pos.z);

					string output = "Successfully created " + to_string(ID) + " at position (" + posX + ", " + posY + ", " + posZ + ")\n";
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::SUCCESS,
						output);
				}
				if (ImGui::MenuItem("Sphere"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light source"))
			{
				if (ImGui::MenuItem("Point light"))
				{

				}
				if (ImGui::MenuItem("Spotlight"))
				{

				}
				if (ImGui::MenuItem("Directional light"))
				{

				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(160 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Debug menu"))
			{
				GUIDebugMenu::renderDebugMenu = true;
			}

			if (ImGui::MenuItem("Console"))
			{
				GUIConsole::renderConsole = true;
			}

			if (ImGui::MenuItem("Inspector"))
			{
				GUIInspector::renderInspector = true;
			}

			if (ImGui::MenuItem("Project hierarchy"))
			{
				GUIProjectHierarchy::renderProjectHierarchy = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(240 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Check for Updates"))
			{
				TB_CheckVersion();
			}

			if (ImGui::MenuItem("Report an Issue"))
			{
				TB_ReportIssue();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void EngineGUI::TB_CheckVersion()
	{
		string batFilePath = Engine::enginePath + "/bat scripts/checkVersion.bat";

		if (exists(batFilePath))
		{
			string batOutput = File::GetOutputFromBatFile((batFilePath).c_str());

			vector<string>splitOutput = String::Split(batOutput, ' ');
			vector<string>cleanedOutput = String::RemoveExcept(splitOutput, "0.0.");
			vector<string>sortedOutput = String::RemoveDuplicates(cleanedOutput);

			string currentVersion = String::StringReplace(Engine::version, " Prototype", "");

			int versionNumber = stoi(String::Split(currentVersion, '.')[2]);
			int currentVersionNumber = 0;
			for (const auto& part : sortedOutput)
			{
				int partVersionNumber = stoi(String::Split(part, '.')[2]);
				if (partVersionNumber > currentVersionNumber)
				{
					currentVersionNumber = partVersionNumber;
				}
			}

			if (currentVersionNumber > versionNumber) outdatedVersion = true;

			versionCompare = 
				"Latest version: 0.0." + to_string(currentVersionNumber) + "\n" +
				" Your version: " + currentVersion;

			versionConfirm += outdatedVersion ?
				"\n\nYour version is out of date!" :
				"\n\nYour version is up to date!";

			showVersionWindow = true;
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Path " + batFilePath + " does not exist!");
		}
	}

	void EngineGUI::TB_ReportIssue()
	{
		try
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::DEBUG,
				"User opened link to Github repository issues page.\n");
			Browser::OpenLink("https://github.com/Lost-Empire-Entertainment/Elypso-engine/issues");
		}
		catch (const exception& e)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Failed to open link to Github repository issues page! " + string(e.what()) + "\n");
		}
	}

	void EngineGUI::RenderVersionCheckWindow()
	{
		ImVec2 initialPos(400, 200);
		ImVec2 initialSize(400, 400);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		if (showVersionWindow
			&& ImGui::Begin("Version", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showVersionWindow = false;
			}

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(versionCompare.c_str()).x) * 0.48f);
			ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::CalcTextSize(versionCompare.c_str()).y) * 0.35f);
			ImGui::Text(versionCompare.c_str());

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(versionConfirm.c_str()).x) * 0.48f);
			ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::CalcTextSize(versionConfirm.c_str()).y) * 0.45f);
			ImGui::Text(versionConfirm.c_str());

			if (outdatedVersion)
			{
				ImVec2 buttonSize(200, 50);

				//move button to center and slightly down
				ImVec2 buttonPos
				(
					(ImGui::GetWindowWidth() - buttonSize.x) * 0.5f,
					(ImGui::GetWindowHeight() - buttonSize.y) * 0.7f
				);

				ImGui::SetCursorPos(buttonPos);

				if (ImGui::Button("Get the latest version", buttonSize))
				{
					try
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::DEBUG,
							"User opened link to Github repository releases page.\n");
						Browser::OpenLink("https://github.com/Lost-Empire-Entertainment/Elypso-engine/releases");
					}
					catch (const exception& e)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::EXCEPTION,
							"Error: Failed to open link to Github repository releases page! " + string(e.what()) + "\n");
					}
				}
			}

			ImGui::End();
		}
	}

	void EngineGUI::Shutdown()
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
	}
}