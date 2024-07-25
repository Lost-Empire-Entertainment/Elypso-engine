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
#include "gui_floatingdebugmenu.hpp"
#include "gui_console.hpp"
#include "gui_settings.hpp"
#include "gui_inspector.hpp"
#include "gui_importAsset.hpp"
#include "gui_scenemenu.hpp"
#include "gui_scenehierarchy.hpp"
#include "gui_projecthierarchy.hpp"
#include "gui_createscene.hpp"
#include "gui_rename.hpp"
#include "gui_credits.hpp"
#include "gui_links.hpp"
#include "gui_projectitemslist.hpp"
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
#include "compile.hpp"
#include "configFile.hpp"

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
using EngineFile::ConfigFile;
using EngineFile::FileExplorer;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Compilation;

namespace Graphics::GUI
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

	void EngineGUI::Initialize()
	{
		//copies template file to documents folder if imgui file does not exist
		string imguiConfigFile = Engine::docsPath + "\\imgui.ini";
		string imguiTemplateFile = Engine::filesPath + "\\imgui.ini";
		if (!exists(imguiConfigFile))
		{
			File::CopyFileOrFolder(imguiTemplateFile, imguiConfigFile);

			ConfigFile::SetValue("gui_sceneHierarchy", "1");
			ConfigFile::SetValue("gui_projectHierarchy", "1");
			ConfigFile::SetValue("gui_console", "1");
			ConfigFile::SetValue("gui_inspector", "1");
			ConfigFile::SaveConfigFile();
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Engine::docsPath + "\\imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::filesPath + "\\fonts\\coda\\Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		io.FontGlobalScale = stof(ConfigFile::GetValue("gui_fontScale"));
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

		if (!Compilation::renderBuildingWindow) RenderTopBar();

		ImGuiDockNodeFlags dockFlags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

		if (!Compilation::renderBuildingWindow)
		{
			GUIFloatingDebugMenu::RenderFloatingDebugMenu();
			GUIConsole::RenderConsole();
			GUISettings::RenderSettings();
			GUIInspector::RenderInspector();
			GUIImportAsset::RenderImportAsset();
			GUISceneMenu::RenderSceneMenu();
			GUISceneHierarchy::RenderSceneHierarchy();
			GUIProjectHierarchy::RenderProjectHierarchy();
			GUICreateScene::RenderCreateSceneWindow();
			GUIRename::RenderRenameWindow();
			GUICredits::RenderCreditsWindow();
			GUILinks::RenderLinksWindow();
			GUIProjectItemsList::RenderProjectItemsList();
		}
		Compilation::RenderBuildingWindow();

		RenderVersionCheckWindow();
		if (renderUnsavedShutdownWindow) ConfirmUnsavedShutdown();
		if (renderUnsavedSceneSwitchWindow) ConfirmUnsavedSceneSwitch();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EngineGUI::RenderTopBar()
	{
		float fontScale = stof(ConfigFile::GetValue("gui_fontScale"));

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SceneFile::SaveScene();
				ConfigFile::SaveConfigFile();
			}

			if (ImGui::MenuItem("New Scene"))
			{
				GUICreateScene::renderCreateSceneWindow = true;
			}

			if (ImGui::MenuItem("Open scene"))
			{
				GUIProjectItemsList::type = GUIProjectItemsList::Type::Scenes;
				GUIProjectItemsList::renderProjectItemsList = true;
			}

			if (ImGui::MenuItem("Exit")) Engine::Shutdown();

			ImGui::EndMenu();
		}

		ImGui::SameLine(40 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Asset"))
		{
			if (ImGui::MenuItem("Import asset"))
			{
				string assetPath = FileExplorer::Select(FileExplorer::SearchType::asset);
				if (assetPath == "")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Did not get path!\n");
				}
				assetPath = String::CharReplace(assetPath, '/', '\\');

				string name = path(assetPath).stem().string();
				string extension = path(assetPath).extension().string();
				if (extension != ".fbx"
					&& extension != ".gltw"
					&& extension != ".obj"
					&& extension != ".png"
					&& extension != ".jpg"
					&& extension != ".jpeg")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"File '" + name + "' with extension '" + extension + "' is not yet supported!");
				}
				else
				{
					bool foundExisting = false;
					string existingFilePath;
					string existingFileName;
					string importedFileName = path(assetPath).stem().string();
					for (const auto& file : directory_iterator(Engine::gameobjectsPath))
					{
						existingFilePath = file.path().string();
						existingFileName = path(existingFilePath).stem().string();

						if (importedFileName == existingFileName)
						{
							foundExisting = true;
							break;
						}
					}

					if (foundExisting)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::FILE,
							Type::EXCEPTION,
							"File '" + importedFileName + "' already exists in the engine!\n");
					}
					else
					{
						GUIImportAsset::renderImportAsset = true;
						GUIImportAsset::assetPath = assetPath;
					}
				}
			}

			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					string originPath = Engine::filesPath + "\\models\\cube.fbx";
					string targetPath = File::AddIndex(Engine::gameobjectsPath, "Cube", "");
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\Cube.fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						destinationPath,
						Engine::filesPath + "\\shaders\\GameObject.vert",
						Engine::filesPath + "\\shaders\\GameObject.frag",
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					string originPath = Engine::filesPath + "\\models\\sphere.fbx";
					string targetPath = File::AddIndex(Engine::gameobjectsPath, "Sphere", "");
					string targetName = path(targetPath).stem().string();
					 
					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\Sphere.fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						destinationPath,
						Engine::filesPath + "\\shaders\\GameObject.vert",
						Engine::filesPath + "\\shaders\\GameObject.frag",
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					string originPath = Engine::filesPath + "\\models\\cylinder.fbx";
					string targetPath = File::AddIndex(Engine::gameobjectsPath, "Cylinder", "");
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\Cylinder.fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						destinationPath,
						Engine::filesPath + "\\shaders\\GameObject.vert",
						Engine::filesPath + "\\shaders\\GameObject.frag",
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Cone"))
				{
					string originPath = Engine::filesPath + "\\models\\cone.fbx";
					string targetPath = File::AddIndex(Engine::gameobjectsPath, "Cone", "");
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\Cone.fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						destinationPath,
						Engine::filesPath + "\\shaders\\GameObject.vert",
						Engine::filesPath + "\\shaders\\GameObject.frag",
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Pyramid"))
				{
					string originPath = Engine::filesPath + "\\models\\pyramid.fbx";
					string targetPath = File::AddIndex(Engine::gameobjectsPath, "Pyramid", "");
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\Pyramid.fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						destinationPath,
						Engine::filesPath + "\\shaders\\GameObject.vert",
						Engine::filesPath + "\\shaders\\GameObject.frag",
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					SceneFile::SaveScene();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light source"))
			{
				if (ImGui::MenuItem("Point light"))
				{
					shared_ptr<GameObject> obj = PointLight::InitializePointLight();

					Select::selectedObj = obj;
					Select::isObjectSelected = true;

					SceneFile::SaveScene();
				}
				if (ImGui::MenuItem("Spotlight"))
				{
					shared_ptr<GameObject> obj = SpotLight::InitializeSpotLight();

					Select::selectedObj = obj;
					Select::isObjectSelected = true;

					SceneFile::SaveScene();
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
				ConfigFile::SetValue("gui_sceneHierarchy", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Project hierarchy"))
			{
				ConfigFile::SetValue("gui_projectHierarchy", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Inspector"))
			{
				ConfigFile::SetValue("gui_inspector", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Scene menu"))
			{
				ConfigFile::SetValue("gui_sceneMenu", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Console"))
			{
				ConfigFile::SetValue("gui_console", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(170 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::IsItemClicked())
			{
				GUISettings::renderSettings = true;

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::SameLine(240 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Compile"))
		{
			if (ImGui::IsItemClicked())
			{
				Compilation::Compile();

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::SameLine(310 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Run"))
		{
			if (ImGui::IsItemClicked())
			{
				if (!exists(Engine::gameExePath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Game exe does not exist!\n");
				}
				else File::RunApplication(Engine::gameParentPath, Engine::gameExePath);

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 225 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Links"))
		{
			if (ImGui::IsItemClicked())
			{
				GUILinks::renderLinksWindow = true;

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 175 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Credits"))
		{
			if (ImGui::IsItemClicked())
			{
				GUICredits::renderCreditsWindow = true;

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 110 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Report issue"))
		{
			if (ImGui::IsItemClicked())
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

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
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
				Caller::INPUT,
				Type::INFO,
				"Closed engine without saving.\n");
			SceneFile::unsavedChanges = false;
			Engine::Shutdown();
		}

		ImGui::SetCursorPos(button3Pos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
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
				Caller::INPUT,
				Type::INFO,
				"Switched scene without saving.\n");
			SceneFile::LoadScene(targetScene);
			renderUnsavedSceneSwitchWindow = false;
		}

		ImGui::SetCursorPos(button3Pos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
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
}