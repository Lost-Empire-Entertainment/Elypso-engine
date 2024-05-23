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
#include "gui_nodeblock.hpp"
#include "gui_assetlist.hpp"
#include "gui_importAsset.hpp"
#include "gui_scenemenu.hpp"
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
using Core::Compilation;

namespace Graphics::GUI
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

	void EngineGUI::Initialize()
	{
		//copies template file to documents folder if imgui file does not exist
		string imguiConfigFile = Engine::docsPath + "/imgui.ini";
		string imguiTemplateFile = Engine::filesPath + "/imgui.ini";
		if (!exists(imguiConfigFile))
		{
			File::CopyFileOrFolder(imguiTemplateFile, imguiConfigFile);

			GUIAssetList::renderAssetList = true;
			GUIConsole::renderConsole = true;
			GUIInspector::renderInspector = true;
			ConfigFileManager::SaveConfigFile();
		}

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
		io.FontGlobalScale = fontScale;

		//
		// GUI STYLE START
		//

		style.Alpha = gui_Alpha;
		style.DisabledAlpha = gui_DisabledAlpha;
		style.WindowPadding = gui_WindowPadding;
		style.WindowRounding = gui_WindowRounding;
		style.WindowBorderSize = gui_WindowBorderSize;
		style.WindowMinSize = gui_WindowMinSize;
		style.WindowTitleAlign = gui_WindowTitleAlign;
		style.WindowMenuButtonPosition = gui_WindowMenuButtonPosition;
		style.ChildRounding = gui_ChildRounding;
		style.ChildBorderSize = gui_ChildBorderSize;
		style.PopupRounding = gui_PopupRounding;
		style.PopupBorderSize = gui_PopupBorderSize;
		style.FramePadding = gui_FramePadding;
		style.FrameRounding = gui_FrameRounding;
		style.FrameBorderSize = gui_FrameBorderSize;
		style.ItemSpacing = gui_ItemSpacing;
		style.ItemInnerSpacing = gui_ItemInnerSpacing;
		style.CellPadding = gui_CellPadding;
		style.IndentSpacing = gui_IndentSpacing;
		style.ColumnsMinSpacing = gui_ColumnsMinSpacing;
		style.ScrollbarSize = gui_ScrollbarSize;
		style.ScrollbarRounding = gui_ScrollbarRounding;
		style.GrabMinSize = gui_GrabMinSize;
		style.GrabRounding = gui_GrabRounding;
		style.TabRounding = gui_TabRounding;
		style.TabBorderSize = gui_TabBorderSize;
		style.TabMinWidthForCloseButton = gui_TabMinWidthForCloseButton;
		style.ColorButtonPosition = gui_ColorButtonPosition;
		style.ButtonTextAlign = gui_ButtonTextAlign;
		style.SelectableTextAlign = gui_SelectableTextAlign;

		style.Colors[ImGuiCol_Text] = gui_Color_Text;
		style.Colors[ImGuiCol_TextDisabled] = gui_Color_TextDisabled;
		style.Colors[ImGuiCol_WindowBg] = gui_Color_WindowBg;
		style.Colors[ImGuiCol_ChildBg] = gui_Color_ChildBg;
		style.Colors[ImGuiCol_PopupBg] = gui_Color_PopupBg;
		style.Colors[ImGuiCol_Border] = gui_Color_Border;
		style.Colors[ImGuiCol_BorderShadow] = gui_Color_BorderShadow;
		style.Colors[ImGuiCol_FrameBg] = gui_Color_FrameBg;
		style.Colors[ImGuiCol_FrameBgHovered] = gui_Color_FrameBgHovered;
		style.Colors[ImGuiCol_FrameBgActive] = gui_Color_FrameBgActive;
		style.Colors[ImGuiCol_TitleBg] = gui_Color_TitleBg;
		style.Colors[ImGuiCol_TitleBgActive] = gui_Color_TitleBgActive;
		style.Colors[ImGuiCol_TitleBgCollapsed] = gui_Color_TitleBgCollapsed;
		style.Colors[ImGuiCol_MenuBarBg] = gui_Color_MenuBarBg;
		style.Colors[ImGuiCol_ScrollbarBg] = gui_Color_ScrollbarBg;
		style.Colors[ImGuiCol_ScrollbarGrab] = gui_Color_ScrollbarGrab;
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = gui_Color_ScrollbarGrabHovered;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = gui_Color_ScrollbarGrabActive;
		style.Colors[ImGuiCol_CheckMark] = gui_Color_CheckMark;
		style.Colors[ImGuiCol_SliderGrab] = gui_Color_SliderGrab;
		style.Colors[ImGuiCol_SliderGrabActive] = gui_Color_SliderGrabActive;
		style.Colors[ImGuiCol_Button] = gui_Color_Button;
		style.Colors[ImGuiCol_ButtonHovered] = gui_Color_ButtonHovered;
		style.Colors[ImGuiCol_ButtonActive] = gui_Color_ButtonActive;
		style.Colors[ImGuiCol_Header] = gui_Color_Header;
		style.Colors[ImGuiCol_HeaderHovered] = gui_Color_HeaderHovered;
		style.Colors[ImGuiCol_HeaderActive] = gui_Color_HeaderActive;
		style.Colors[ImGuiCol_Separator] = gui_Color_Separator;
		style.Colors[ImGuiCol_SeparatorHovered] = gui_Color_SeparatorHovered;
		style.Colors[ImGuiCol_SeparatorActive] = gui_Color_SeparatorActive;
		style.Colors[ImGuiCol_ResizeGrip] = gui_Color_ResizeGrip;
		style.Colors[ImGuiCol_ResizeGripHovered] = gui_Color_ResizeGripHovered;
		style.Colors[ImGuiCol_ResizeGripActive] = gui_Color_ResizeGripActive;
		style.Colors[ImGuiCol_Tab] = gui_Color_Tab;
		style.Colors[ImGuiCol_TabHovered] = gui_Color_TabHovered;
		style.Colors[ImGuiCol_TabActive] = gui_Color_TabActive;
		style.Colors[ImGuiCol_TabUnfocused] = gui_Color_TabUnfocused;
		style.Colors[ImGuiCol_TabUnfocusedActive] = gui_Color_TabUnfocusedActive;
		style.Colors[ImGuiCol_PlotLines] = gui_Color_PlotLines;
		style.Colors[ImGuiCol_PlotLinesHovered] = gui_Color_PlotLinesHovered;
		style.Colors[ImGuiCol_PlotHistogram] = gui_Color_PlotHistogram;
		style.Colors[ImGuiCol_PlotHistogramHovered] = gui_Color_PlotHistogramHovered;
		style.Colors[ImGuiCol_TableHeaderBg] = gui_Color_TableHeaderBg;
		style.Colors[ImGuiCol_TableBorderStrong] = gui_Color_TableBorderStrong;
		style.Colors[ImGuiCol_TableBorderLight] = gui_Color_TableBorderLight;
		style.Colors[ImGuiCol_TableRowBg] = gui_Color_TableRowBg;
		style.Colors[ImGuiCol_TableRowBgAlt] = gui_Color_TableRowBgAlt;
		style.Colors[ImGuiCol_TextSelectedBg] = gui_Color_TextSelectedBg;
		style.Colors[ImGuiCol_DragDropTarget] = gui_Color_DragDropTarget;
		style.Colors[ImGuiCol_NavHighlight] = gui_Color_NavHighlight;
		style.Colors[ImGuiCol_NavWindowingHighlight] = gui_Color_NavWindowingHighlight;
		style.Colors[ImGuiCol_NavWindowingDimBg] = gui_Color_NavWindowingDimBg;
		style.Colors[ImGuiCol_ModalWindowDimBg] = gui_Color_ModalWindowDimBg;

		//
		// GUI STYLE END
		//
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

		GUIFloatingDebugMenu::RenderFloatingDebugMenu();
		GUIConsole::RenderConsole();
		GUISettings::RenderSettings();
		GUIInspector::RenderInspector();
		GUINodeBlock::RenderNodeBlock();
		GUIAssetList::RenderAssetList();
		GUIImportAsset::RenderImportAsset();
		GUISceneMenu::RenderSceneMenu();

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

			if (ImGui::MenuItem("Import asset"))
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
						GUIImportAsset::renderImportAsset = true;
						GUIImportAsset::assetPath = path;
					}
				}
			}

			map<GameObject::Category, bool> categories;
			for (const auto& category : GameObject::categoriesVector)
			{
				categories[category] = category == GameObject::Category::cat_All ? true : false;
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
						categories[GameObject::Category::cat_Props_Static_props] = true;

						string targetPath = Engine::filesPath + "/models/cube.fbx";
						string targetName = "Cube";
						Model::Initialize(
							vec3(0),
							vec3(0),
							vec3(1),
							targetPath,
							Engine::filesPath + "/shaders/GameObject.vert",
							Engine::filesPath + "/shaders/GameObject.frag",
							Engine::filesPath + "/textures/default_diffuse.png",
							"EMPTY",
							"EMPTY",
							"EMPTY",
							32,
							categories,
							targetName,
							Model::tempID);

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
						categories[GameObject::Category::cat_Props_Static_props] = true;

						string targetPath = Engine::filesPath + "/models/sphere.fbx";
						string targetName = "Sphere";
						Model::Initialize(
							vec3(0),
							vec3(0),
							vec3(1),
							targetPath,
							Engine::filesPath + "/shaders/GameObject.vert",
							Engine::filesPath + "/shaders/GameObject.frag",
							Engine::filesPath + "/textures/default_diffuse.png",
							"EMPTY",
							"EMPTY",
							"EMPTY",
							32,
							categories,
							targetName,
							Model::tempID);

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
						categories[GameObject::Category::cat_Props_Static_props] = true;

						string targetPath = Engine::filesPath + "/models/cylinder.fbx";
						string targetName = "Cylinder";
						Model::Initialize(
							vec3(0),
							vec3(0),
							vec3(1),
							targetPath,
							Engine::filesPath + "/shaders/GameObject.vert",
							Engine::filesPath + "/shaders/GameObject.frag",
							Engine::filesPath + "/textures/default_diffuse.png",
							"EMPTY",
							"EMPTY",
							"EMPTY",
							32,
							categories,
							targetName,
							Model::tempID);

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
						categories[GameObject::Category::cat_Props_Static_props] = true;

						string targetPath = Engine::filesPath + "/models/cone.fbx";
						string targetName = "Cone";
						Model::Initialize(
							vec3(0),
							vec3(0),
							vec3(1),
							targetPath,
							Engine::filesPath + "/shaders/GameObject.vert",
							Engine::filesPath + "/shaders/GameObject.frag",
							Engine::filesPath + "/textures/default_diffuse.png",
							"EMPTY",
							"EMPTY",
							"EMPTY",
							32,
							categories,
							targetName,
							Model::tempID);

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
						categories[GameObject::Category::cat_Props_Static_props] = true;

						string targetPath = Engine::filesPath + "/models/pyramid.fbx";
						string targetName = "Pyramid";
						Model::Initialize(
							vec3(0),
							vec3(0),
							vec3(1),
							targetPath,
							Engine::filesPath + "/shaders/GameObject.vert",
							Engine::filesPath + "/shaders/GameObject.frag",
							Engine::filesPath + "/textures/default_diffuse.png",
							"EMPTY",
							"EMPTY",
							"EMPTY",
							32,
							categories,
							targetName,
							Model::tempID);

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

						obj->SetCategoriesMap(categories);
						obj->SetCategoryState(GameObject::Category::cat_Lights_Point_lights, true);

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

						obj->SetCategoriesMap(categories);
						obj->SetCategoryState(GameObject::Category::cat_Lights_Spotlights, true);

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
			if (ImGui::MenuItem("Asset list"))
			{
				GUIAssetList::renderAssetList = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Node block"))
			{
				GUINodeBlock::renderNodeBlock = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Inspector"))
			{
				GUIInspector::renderInspector = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Scene menu"))
			{
				GUISceneMenu::renderSceneMenu = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Console"))
			{
				GUIConsole::renderConsole = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Settings"))
			{
				GUISettings::renderSettings = true;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
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
				Compilation::Compile();

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
}