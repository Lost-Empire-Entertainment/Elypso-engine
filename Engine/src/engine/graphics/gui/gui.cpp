//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_debugmenu.hpp"
#include "gui_inspector.hpp"
#include "gui_projecthierarchy.hpp"
#include "gui_scenehierarchy.hpp"
#include "gui_nodeblock.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "browserUtils.hpp"
#include "gameobject.hpp"
#include "timeManager.hpp"
#include "model.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "selectobject.hpp"
#include "sceneFile.hpp"
#include "configFile.hpp"
#include "fileexplorer.hpp"

using std::cout;
using std::endl;
using std::stoi;
using std::to_string;
using std::exception;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using glm::clamp;
using glm::vec3;

using Core::Engine;
using Core::Input;
using Core::ConsoleManager;
using Core::TimeManager;
using Physics::Select;
using Utils::File;
using Utils::Browser;
using Utils::String;
using Graphics::Shape::Model;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using EngineFile::SceneFile;
using EngineFile::ConfigFileManager;
using EngineFile::FileExplorer;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

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

		initialPos = ImVec2(200, 150);
		initialSize = ImVec2(400, 700);
		minSize = ImVec2(400, 400);

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::filesPath + "/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		CustomizeImGuiStyle();

		GUINodeBlock::SetBackgroundTexture();
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

		ImGuiIO& io = ImGui::GetIO();
		maxSize = ImVec2(io.DisplaySize.x, io.DisplaySize.y - 200);

		RenderTopBar();

		ImGuiDockNodeFlags dockFlags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

		GUIConsole::RenderConsole();
		GUIDebugMenu::RenderDebugMenu();
		GUIInspector::RenderInspector();
		GUIProjectHierarchy::RenderProjectHierarchy(Engine::filesPath);
		GUISceneHierarchy::RenderSceneHierarchy();
		GUINodeBlock::RenderNodeBlock();
		if (GUIProjectHierarchy::rootPath.empty())
		{
			GUIProjectHierarchy::rootPath = Engine::filesPath;
		}

		RenderVersionCheckWindow();
		if (renderUnsavedShutdownWindow) ConfirmUnsavedShutdown();
		if (renderUnsavedSceneSwitchWindow) ConfirmUnsavedSceneSwitch();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EngineGUI::RenderTopBar()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SceneFile::SaveScene();
				ConfigFileManager::SaveConfigFile();
			}

			if (ImGui::MenuItem("New Scene"))
			{
				SceneFile::CreateScene();
			}

			if (ImGui::MenuItem("Open scene"))
			{
				string scenePath = FileExplorer::Select(FileExplorer::SearchType::txt);
				SceneFile::LoadScene(scenePath);
			}

			if (ImGui::MenuItem("Exit")) Engine::Shutdown();

			ImGui::EndMenu();
		}

		ImGui::SameLine(40 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Asset"))
		{
			bool canPlaceAssets = SceneFile::currentScenePath != "";

			if (ImGui::MenuItem("Import model"))
			{
				if (!canPlaceAssets)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot place assets because no scene is loaded!\n");
				}
				else
				{
					string path = FileExplorer::Select(FileExplorer::SearchType::asset);
					if (path == "")
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Did not get path!\n");
					}
					else
					{
						Model::targetModel = path;
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
			}

			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						Model::targetModel = Engine::filesPath + "/models/cube.fbx";
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						Model::targetModel = Engine::filesPath + "/models/sphere.fbx";
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						Model::targetModel = Engine::filesPath + "/models/cylinder.fbx";
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
				else if (ImGui::MenuItem("Cone"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						Model::targetModel = Engine::filesPath + "/models/cone.fbx";
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
				else if (ImGui::MenuItem("Pyramid"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						Model::targetModel = Engine::filesPath + "/models/pyramid.fbx";
						Model::Initialize();

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light source"))
			{
				if (ImGui::MenuItem("Point light"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						shared_ptr<GameObject> obj = PointLight::InitializePointLight();

						Select::selectedObj = obj;
						Select::isObjectSelected = true;

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
				if (ImGui::MenuItem("Spotlight"))
				{
					if (!canPlaceAssets)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Cannot place assets because no scene is loaded!\n");
					}
					else
					{
						shared_ptr<GameObject> obj = SpotLight::InitializeSpotLight();

						Select::selectedObj = obj;
						Select::isObjectSelected = true;

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(100 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Scene hierarchy"))
			{
				GUISceneHierarchy::renderSceneHierarchy = true;
			}

			if (ImGui::MenuItem("Project hierarchy"))
			{
				GUIProjectHierarchy::renderProjectHierarchy = true;
			}

			if (ImGui::MenuItem("Node block"))
			{
				GUINodeBlock::renderNodeBlock = true;
			}

			if (ImGui::MenuItem("Inspector"))
			{
				GUIInspector::renderInspector = true;
			}

			if (ImGui::MenuItem("Console"))
			{
				GUIConsole::renderConsole = true;
			}

			if (ImGui::MenuItem("Debug menu"))
			{
				GUIDebugMenu::renderDebugMenu = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(180 * fontScale * 0.75f);

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

		ImGui::SameLine(240 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Compile"))
		{
			if (ImGui::IsItemClicked())
			{
				EngineGUI::Compile();

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::SameLine(320 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Run"))
		{
			if (ImGui::IsItemClicked())
			{
				if (!exists(Engine::gameExePath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Game exe does not exist!\n");
				}
				else File::RunApplication(Engine::gameParentPath, Engine::gameExePath);

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}

	void EngineGUI::TB_CheckVersion()
	{
		string batFilePath = Engine::filesPath + "/bat scripts/checkVersion.bat";

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
				"Path " + batFilePath + " does not exist!\n\n");
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
				"Failed to open link to Github repository issues page! " + string(e.what()) + "\n");
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
							"Failed to open link to Github repository releases page! " + string(e.what()) + "\n");
					}
				}
			}

			ImGui::End();
		}
	}

	void EngineGUI::ConfirmUnsavedShutdown()
	{
		ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(500, 300));

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings;

		string title = "You have unsaved changes. Save before closing?";
		ImGui::Begin(title.c_str(), nullptr, flags);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 windowCenter(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
		ImVec2 buttonSize(120, 50);
		float buttonSpacing = 20.0f;

		ImVec2 button1Pos(
			windowSize.x * 0.3f - buttonSize.x,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);
		ImVec2 button2Pos(
			windowSize.x * 0.625f - buttonSize.x,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);
		ImVec2 button3Pos(
			windowSize.x * 0.7f,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);

		ImGui::SetCursorPos(button1Pos);
		if (ImGui::Button("Save", buttonSize))
		{
			SceneFile::SaveScene(SceneFile::SaveType::shutDown);
			renderUnsavedShutdownWindow = false;
		}

		ImGui::SetCursorPos(button2Pos);
		if (ImGui::Button("Don't save", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Closed engine without saving.\n");
			SceneFile::unsavedChanges = false;
			Engine::Shutdown();
		}

		ImGui::SetCursorPos(button3Pos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Cancelled shutdown...\n");
			renderUnsavedShutdownWindow = false;
		}

		ImGui::End();
	}

	void EngineGUI::ConfirmUnsavedSceneSwitch()
	{
		ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(500, 300));

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings;

		string title = "You have unsaved changes. Save before switching scenes?";
		ImGui::Begin(title.c_str(), nullptr, flags);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 windowCenter(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
		ImVec2 buttonSize(120, 50);
		float buttonSpacing = 20.0f;

		ImVec2 button1Pos(
			windowSize.x * 0.3f - buttonSize.x,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);
		ImVec2 button2Pos(
			windowSize.x * 0.625f - buttonSize.x,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);
		ImVec2 button3Pos(
			windowSize.x * 0.7f,
			windowSize.y * 0.5f - buttonSize.y * 0.5f);

		ImGui::SetCursorPos(button1Pos);
		if (ImGui::Button("Save", buttonSize))
		{
			SceneFile::SaveScene(SceneFile::SaveType::sceneSwitch, targetScene);
			renderUnsavedSceneSwitchWindow = false;
		}

		ImGui::SetCursorPos(button2Pos);
		if (ImGui::Button("Don't save", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Switched scene without saving.\n");
			SceneFile::LoadScene(targetScene);
			renderUnsavedSceneSwitchWindow = false;
		}

		ImGui::SetCursorPos(button3Pos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Cancelled scene switch...\n");
			renderUnsavedSceneSwitchWindow = false;
		}

		ImGui::End();
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

	void EngineGUI::Compile()
	{
		try
		{
			if (Engine::gamePath == ""
				|| Engine::gameExePath == ""
				|| Engine::gameParentPath == "")
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Game path has not been set!\n");
			}
			else
			{
				string gameBatPath = Engine::gamePath + "\\build_engine_fast.bat";
				gameBatPath = String::CharReplace(gameBatPath, '/', '\\');

				int result = File::RunBatFile(gameBatPath);

				if (result != 0)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Compilation failed! Trying again...\n");

					string gameBatPath = Engine::gamePath + "\\build_engine_full.bat";
					gameBatPath = String::CharReplace(gameBatPath, '/', '\\');

					int result = File::RunBatFile(gameBatPath);

					if (result != 0)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Compilation failed again! Something is broken, please contact engine developers...\n");

						return;
					}
				}

				string gameProjectFolder = Engine::gameParentPath + "\\files\\project";
				for (const auto& item : directory_iterator(path(gameProjectFolder)))
				{
					if (is_directory(item.path())
						|| is_regular_file(item.path()))
					{
						string itemPath = item.path().string();

						cout << "attempting to delete\n" << itemPath << "\n";
						File::DeleteFileOrfolder(itemPath);
					}
				}

				string engineProjectPath = path(Engine::filesPath).parent_path().string() + "\\project";
				for (const auto& item : directory_iterator(path(engineProjectPath)))
				{
					string itemPath = item.path().string();

					string ending = item.is_directory()
						? path(item).filename().string()
						: path(item).stem().string() + path(item).extension().string();
					string targetItemPath = gameProjectFolder + "\\" + ending;

					cout << "attempting to copy\n" << itemPath << "\nto\n" << targetItemPath << "\n";
					File::CopyFileOrFolder(itemPath, targetItemPath);
				}

				cout << "attempting to run\n" << Engine::gameExePath << "\nwhose parent is\n" << Engine::gameParentPath << "\n";
				File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			}
		}
		catch (const exception& e)
		{
			cout << e.what() << "\n";
		}
	}
}