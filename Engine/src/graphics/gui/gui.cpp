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
using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;
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

			ConfigFileManager::valuesMap["gui_sceneHierarchy"].SetValue("1");
			ConfigFileManager::valuesMap["gui_projectHierarchy"].SetValue("1");
			ConfigFileManager::valuesMap["gui_console"].SetValue("1");
			ConfigFileManager::valuesMap["gui_inspector"].SetValue("1");
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

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::filesPath + "/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		CustomizeImGuiStyle();
	}
	void EngineGUI::AssignGuiColorValue(const string& name, ImGuiCol col)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ConfigFileValue valueConfig = ConfigFileManager::valuesMap[name];
		string value = valueConfig.GetValue();
		vector<string> valueSplit = String::Split(value, ',');
		ImVec4 finalValue = ImVec4(
			stof(valueSplit[0]),
			stof(valueSplit[1]),
			stof(valueSplit[2]),
			stof(valueSplit[3]));

		style.Colors[col] = finalValue;
	}
	void EngineGUI::CustomizeImGuiStyle()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = stof(ConfigFileManager::valuesMap["gui_fontScale"].GetValue());

		//
		// GUI STYLE START
		//

		style.Alpha = stof(ConfigFileManager::valuesMap["gui_Alpha"].GetValue());
		style.DisabledAlpha = stof(ConfigFileManager::valuesMap["gui_DisabledAlpha"].GetValue());

		string gui_WindowPadding_string = ConfigFileManager::valuesMap["gui_WindowPadding"].GetValue();
		vector<string> gui_WindowPadding_split = String::Split(gui_WindowPadding_string, ',');
		ImVec2 gui_WindowPadding_value = ImVec2(
			stof(gui_WindowPadding_split[0]),
			stof(gui_WindowPadding_split[1]));
		style.WindowPadding = gui_WindowPadding_value;

		style.WindowRounding = stof(ConfigFileManager::valuesMap["gui_WindowRounding"].GetValue());
		style.WindowBorderSize = stof(ConfigFileManager::valuesMap["gui_WindowBorderSize"].GetValue());

		string gui_WindowMenuButtonPosition_string = ConfigFileManager::valuesMap["gui_WindowMenuButtonPosition"].GetValue();
		auto gui_WindowMenuButtonPosition_dir = magic_enum::enum_cast<ImGuiDir_>(gui_WindowMenuButtonPosition_string);
		if (gui_WindowMenuButtonPosition_dir.has_value())
		{
			style.WindowMenuButtonPosition = gui_WindowMenuButtonPosition_dir.value();
		}
		else cout << "'gui_WindowMenuButtonPosition_dir' has no value!\n"; 

		style.ChildRounding = stof(ConfigFileManager::valuesMap["gui_ChildRounding"].GetValue());
		style.ChildBorderSize = stof(ConfigFileManager::valuesMap["gui_ChildBorderSize"].GetValue());
		style.PopupRounding = stof(ConfigFileManager::valuesMap["gui_PopupRounding"].GetValue());
		style.PopupBorderSize = stof(ConfigFileManager::valuesMap["gui_PopupBorderSize"].GetValue());

		string gui_FramePadding_string = ConfigFileManager::valuesMap["gui_FramePadding"].GetValue();
		vector<string> gui_FramePadding_split = String::Split(gui_FramePadding_string, ',');
		ImVec2 gui_FramePadding_value = ImVec2(
			stof(gui_FramePadding_split[0]),
			stof(gui_FramePadding_split[1]));
		style.FramePadding = gui_FramePadding_value;

		style.FrameRounding = stof(ConfigFileManager::valuesMap["gui_FrameRounding"].GetValue());
		style.FrameBorderSize = stof(ConfigFileManager::valuesMap["gui_FrameBorderSize"].GetValue());

		string gui_ItemSpacing_string = ConfigFileManager::valuesMap["gui_ItemSpacing"].GetValue();
		vector<string> gui_ItemSpacing_split = String::Split(gui_ItemSpacing_string, ',');
		ImVec2 gui_ItemSpacing_value = ImVec2(
			stof(gui_ItemSpacing_split[0]),
			stof(gui_ItemSpacing_split[1]));
		style.ItemSpacing = gui_ItemSpacing_value;

		string gui_ItemInnerSpacing_string = ConfigFileManager::valuesMap["gui_ItemInnerSpacing"].GetValue();
		vector<string> gui_ItemInnerSpacing_split = String::Split(gui_ItemInnerSpacing_string, ',');
		ImVec2 gui_ItemInnerSpacing_value = ImVec2(
			stof(gui_ItemInnerSpacing_split[0]),
			stof(gui_ItemInnerSpacing_split[1]));
		style.ItemInnerSpacing = gui_ItemInnerSpacing_value;

		string gui_CellPadding_string = ConfigFileManager::valuesMap["gui_CellPadding"].GetValue();
		vector<string> gui_CellPadding_split = String::Split(gui_CellPadding_string, ',');
		ImVec2 gui_CellPadding_value = ImVec2(
			stof(gui_CellPadding_split[0]),
			stof(gui_CellPadding_split[1]));
		style.CellPadding = gui_CellPadding_value;

		style.IndentSpacing = stof(ConfigFileManager::valuesMap["gui_IndentSpacing"].GetValue());
		style.ColumnsMinSpacing = stof(ConfigFileManager::valuesMap["gui_ColumnsMinSpacing"].GetValue());
		style.ScrollbarSize = stof(ConfigFileManager::valuesMap["gui_ScrollbarSize"].GetValue());
		style.ScrollbarRounding = stof(ConfigFileManager::valuesMap["gui_ScrollbarRounding"].GetValue());
		style.GrabMinSize = stof(ConfigFileManager::valuesMap["gui_GrabMinSize"].GetValue());
		style.GrabRounding = stof(ConfigFileManager::valuesMap["gui_GrabRounding"].GetValue());
		style.TabRounding = stof(ConfigFileManager::valuesMap["gui_TabRounding"].GetValue());
		style.TabBorderSize = stof(ConfigFileManager::valuesMap["gui_TabBorderSize"].GetValue());
		style.TabMinWidthForCloseButton = stof(ConfigFileManager::valuesMap["gui_TabMinWidthForCloseButton"].GetValue());

		string gui_ColorButtonPosition_string = ConfigFileManager::valuesMap["gui_ColorButtonPosition"].GetValue();
		auto gui_ColorButtonPosition_dir = magic_enum::enum_cast<ImGuiDir_>(gui_ColorButtonPosition_string);
		if (gui_ColorButtonPosition_dir.has_value())
		{
			style.ColorButtonPosition = gui_ColorButtonPosition_dir.value();
		}
		else cout << "'gui_ColorButtonPosition_dir' has no value!\n";

		string gui_ButtonTextAlign_string = ConfigFileManager::valuesMap["gui_ButtonTextAlign"].GetValue();
		vector<string> gui_ButtonTextAlign_split = String::Split(gui_ButtonTextAlign_string, ',');
		ImVec2 gui_ButtonTextAlign_value = ImVec2(
			stof(gui_ButtonTextAlign_split[0]),
			stof(gui_ButtonTextAlign_split[1]));
		style.ButtonTextAlign = gui_ButtonTextAlign_value;

		string gui_SelectableTextAlign_string = ConfigFileManager::valuesMap["gui_SelectableTextAlign"].GetValue();
		vector<string> gui_SelectableTextAlign_split = String::Split(gui_SelectableTextAlign_string, ',');
		ImVec2 gui_SelectableTextAlign_value = ImVec2(
			stof(gui_SelectableTextAlign_split[0]),
			stof(gui_SelectableTextAlign_split[1]));
		style.SelectableTextAlign = gui_SelectableTextAlign_value;

		//
		// GUI COLOR START
		//

		AssignGuiColorValue("gui_Color_Text", ImGuiCol_Text);
		AssignGuiColorValue("gui_Color_TextDisabled", ImGuiCol_TextDisabled);
		AssignGuiColorValue("gui_Color_WindowBg", ImGuiCol_WindowBg);
		AssignGuiColorValue("gui_Color_ChildBg", ImGuiCol_ChildBg);
		AssignGuiColorValue("gui_Color_PopupBg", ImGuiCol_PopupBg);
		AssignGuiColorValue("gui_Color_Border", ImGuiCol_Border);
		AssignGuiColorValue("gui_Color_BorderShadow", ImGuiCol_BorderShadow);
		AssignGuiColorValue("gui_Color_FrameBg", ImGuiCol_FrameBg);
		AssignGuiColorValue("gui_Color_FrameBgHovered", ImGuiCol_FrameBgHovered);
		AssignGuiColorValue("gui_Color_FrameBgActive", ImGuiCol_FrameBgActive);
		AssignGuiColorValue("gui_Color_TitleBg", ImGuiCol_TitleBg);

		AssignGuiColorValue("gui_Color_TitleBgActive", ImGuiCol_TitleBgActive);
		AssignGuiColorValue("gui_Color_TitleBgCollapsed", ImGuiCol_TitleBgCollapsed);
		AssignGuiColorValue("gui_Color_MenuBarBg", ImGuiCol_MenuBarBg);
		AssignGuiColorValue("gui_Color_ScrollbarBg", ImGuiCol_ScrollbarBg);
		AssignGuiColorValue("gui_Color_ScrollbarGrab", ImGuiCol_ScrollbarGrab);
		AssignGuiColorValue("gui_Color_ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered);
		AssignGuiColorValue("gui_Color_ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive);
		AssignGuiColorValue("gui_Color_CheckMark", ImGuiCol_CheckMark);
		AssignGuiColorValue("gui_Color_SliderGrab", ImGuiCol_SliderGrab);
		AssignGuiColorValue("gui_Color_SliderGrabActive", ImGuiCol_SliderGrabActive);
		AssignGuiColorValue("gui_Color_Button", ImGuiCol_Button);
		AssignGuiColorValue("gui_Color_ButtonHovered", ImGuiCol_ButtonHovered);
		AssignGuiColorValue("gui_Color_ButtonActive", ImGuiCol_ButtonActive);
		AssignGuiColorValue("gui_Color_Header", ImGuiCol_Header);
		AssignGuiColorValue("gui_Color_HeaderHovered", ImGuiCol_HeaderHovered);

		AssignGuiColorValue("gui_Color_HeaderActive", ImGuiCol_HeaderActive);
		AssignGuiColorValue("gui_Color_Separator", ImGuiCol_Separator);
		AssignGuiColorValue("gui_Color_SeparatorHovered", ImGuiCol_SeparatorHovered);
		AssignGuiColorValue("gui_Color_SeparatorActive", ImGuiCol_SeparatorActive);
		AssignGuiColorValue("gui_Color_ResizeGrip", ImGuiCol_ResizeGrip);
		AssignGuiColorValue("gui_Color_ResizeGripHovered", ImGuiCol_ResizeGripHovered);
		AssignGuiColorValue("gui_Color_ResizeGripActive", ImGuiCol_ResizeGripActive);
		AssignGuiColorValue("gui_Color_Tab", ImGuiCol_Tab);
		AssignGuiColorValue("gui_Color_TabHovered", ImGuiCol_TabHovered);
		AssignGuiColorValue("gui_Color_TabActive", ImGuiCol_TabActive);
		AssignGuiColorValue("gui_Color_TabUnfocused", ImGuiCol_TabUnfocused);
		AssignGuiColorValue("gui_Color_TitleBg", ImGuiCol_TitleBg);
		AssignGuiColorValue("gui_Color_TabUnfocusedActive", ImGuiCol_TabUnfocusedActive);
		AssignGuiColorValue("gui_Color_PlotLines", ImGuiCol_PlotLines);
		AssignGuiColorValue("gui_Color_PlotLinesHovered", ImGuiCol_PlotLinesHovered);
		AssignGuiColorValue("gui_Color_PlotHistogram", ImGuiCol_PlotHistogram);
		AssignGuiColorValue("gui_Color_PlotHistogramHovered", ImGuiCol_PlotHistogramHovered);
		AssignGuiColorValue("gui_Color_TableHeaderBg", ImGuiCol_TableHeaderBg);
		AssignGuiColorValue("gui_Color_TableBorderStrong", ImGuiCol_TableBorderStrong);
		AssignGuiColorValue("gui_Color_TableBorderLight", ImGuiCol_TableBorderLight);
		AssignGuiColorValue("gui_Color_TableRowBg", ImGuiCol_TableRowBg);
		AssignGuiColorValue("gui_Color_TableRowBgAlt", ImGuiCol_TableRowBgAlt);
		AssignGuiColorValue("gui_Color_TextSelectedBg", ImGuiCol_TextSelectedBg);
		AssignGuiColorValue("gui_Color_DragDropTarget", ImGuiCol_DragDropTarget);
		AssignGuiColorValue("gui_Color_NavHighlight", ImGuiCol_NavHighlight);
		AssignGuiColorValue("gui_Color_NavWindowingHighlight", ImGuiCol_NavWindowingHighlight);
		AssignGuiColorValue("gui_Color_NavWindowingDimBg", ImGuiCol_NavWindowingDimBg);
		AssignGuiColorValue("gui_Color_ModalWindowDimBg", ImGuiCol_ModalWindowDimBg);
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

		RenderTopBar();

		ImGuiDockNodeFlags dockFlags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

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

		RenderVersionCheckWindow();
		if (renderUnsavedShutdownWindow) ConfirmUnsavedShutdown();
		if (renderUnsavedSceneSwitchWindow) ConfirmUnsavedSceneSwitch();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EngineGUI::RenderTopBar()
	{
		float fontScale = stof(ConfigFileManager::valuesMap["gui_fontScale"].GetValue());

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
			if (ImGui::MenuItem("Import asset"))
			{
				string assetPath = FileExplorer::Select(FileExplorer::SearchType::asset);
				if (assetPath == "")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Did not get path!\n");
				}

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
						Caller::ENGINE,
						Type::EXCEPTION,
						"File '" + name + "' with extension '" + extension + "' is not yet supported!");
				}
				else
				{
					bool foundExisting = false;
					string existingFilePath;
					string existingFileName;
					string importedFileName = path(assetPath).filename().string();
					for (const auto& file : directory_iterator(Engine::filesPath + "/models"))
					{
						existingFilePath = file.path().string();
						existingFileName = path(existingFilePath).filename().string();

						if (importedFileName == existingFileName)
						{
							cout << "comparing " + importedFileName + "\n" << assetPath
								<< "\nto " + existingFileName + "\n" << existingFilePath << "\n\n";

							foundExisting = true;
							break;
						}
					}

					if (foundExisting)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
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
					string modelsFolder = path(Engine::scenePath).parent_path().string() + "\\models";
					string originPath = Engine::filesPath + "/models/cube.fbx";
					string targetName = "Cube";
					string fullTargetName = "Cube.fbx";
					string targetPath = File::AddIndex(modelsFolder, targetName, "");
					File::CreateNewFolder(targetPath);

					targetName = path(targetPath).stem().string();
					fullTargetName = path(targetPath).stem().string() + ".fbx";

					string fullTargetPath = targetPath + "\\" + fullTargetName;
					File::CopyFileOrFolder(originPath, fullTargetPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						fullTargetPath,
						Engine::filesPath + "/shaders/GameObject.vert",
						Engine::filesPath + "/shaders/GameObject.frag",
						Engine::filesPath + "/textures/diff_default.png",
						"EMPTY",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					string modelsFolder = path(Engine::scenePath).parent_path().string() + "\\models";
					string originPath = Engine::filesPath + "/models/sphere.fbx";
					string targetName = "Sphere";
					string fullTargetName = "Sphere.fbx";
					string targetPath = File::AddIndex(modelsFolder, targetName, "");
					File::CreateNewFolder(targetPath);

					targetName = path(targetPath).stem().string();
					fullTargetName = path(targetPath).stem().string() + ".fbx";

					string fullTargetPath = targetPath + "\\" + fullTargetName;
					File::CopyFileOrFolder(originPath, fullTargetPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						fullTargetPath,
						Engine::filesPath + "/shaders/GameObject.vert",
						Engine::filesPath + "/shaders/GameObject.frag",
						Engine::filesPath + "/textures/diff_default.png",
						"EMPTY",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					string modelsFolder = path(Engine::scenePath).parent_path().string() + "\\models";
					string originPath = Engine::filesPath + "/models/cylinder.fbx";
					string targetName = "Cylinder";
					string fullTargetName = "Cylinder.fbx";
					string targetPath = File::AddIndex(modelsFolder, targetName, "");
					File::CreateNewFolder(targetPath);

					targetName = path(targetPath).stem().string();
					fullTargetName = path(targetPath).stem().string() + ".fbx";

					string fullTargetPath = targetPath + "\\" + fullTargetName;
					File::CopyFileOrFolder(originPath, fullTargetPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						fullTargetPath,
						Engine::filesPath + "/shaders/GameObject.vert",
						Engine::filesPath + "/shaders/GameObject.frag",
						Engine::filesPath + "/textures/diff_default.png",
						"EMPTY",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Cone"))
				{
					string modelsFolder = path(Engine::scenePath).parent_path().string() + "\\models";
					string originPath = Engine::filesPath + "/models/cone.fbx";
					string targetName = "Cone";
					string fullTargetName = "Cone.fbx";
					string targetPath = File::AddIndex(modelsFolder, targetName, "");
					File::CreateNewFolder(targetPath);

					targetName = path(targetPath).stem().string();
					fullTargetName = path(targetPath).stem().string() + ".fbx";

					string fullTargetPath = targetPath + "\\" + fullTargetName;
					File::CopyFileOrFolder(originPath, fullTargetPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						fullTargetPath,
						Engine::filesPath + "/shaders/GameObject.vert",
						Engine::filesPath + "/shaders/GameObject.frag",
						Engine::filesPath + "/textures/diff_default.png",
						"EMPTY",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Pyramid"))
				{
					string modelsFolder = path(Engine::scenePath).parent_path().string() + "\\models";
					string originPath = Engine::filesPath + "/models/pyramid.fbx";
					string targetName = "Pyramid";
					string fullTargetName = "Pyramid.fbx";
					string targetPath = File::AddIndex(modelsFolder, targetName, "");
					File::CreateNewFolder(targetPath);

					targetName = path(targetPath).stem().string();
					fullTargetName = path(targetPath).stem().string() + ".fbx";

					string fullTargetPath = targetPath + "\\" + fullTargetName;
					File::CopyFileOrFolder(originPath, fullTargetPath);

					Model::Initialize(
						vec3(0),
						vec3(0),
						vec3(1),
						fullTargetPath,
						Engine::filesPath + "/shaders/GameObject.vert",
						Engine::filesPath + "/shaders/GameObject.frag",
						Engine::filesPath + "/textures/diff_default.png",
						"EMPTY",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
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

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				if (ImGui::MenuItem("Spotlight"))
				{
					shared_ptr<GameObject> obj = SpotLight::InitializeSpotLight();

					Select::selectedObj = obj;
					Select::isObjectSelected = true;

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
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
				ConfigFileManager::valuesMap["gui_sceneHierarchy"].SetValue("1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Project hierarchy"))
			{
				ConfigFileManager::valuesMap["gui_projectHierarchy"].SetValue("1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Inspector"))
			{
				ConfigFileManager::valuesMap["gui_inspector"].SetValue("1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Scene menu"))
			{
				ConfigFileManager::valuesMap["gui_sceneMenu"].SetValue("1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Console"))
			{
				ConfigFileManager::valuesMap["gui_console"].SetValue("1");
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

		ImGui::SameLine(245 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Create scene"))
		{
			if (ImGui::IsItemClicked())
			{
				GUICreateScene::renderCreateSceneWindow = true;

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::SameLine(350 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Compile"))
		{
			if (ImGui::IsItemClicked())
			{
				Compilation::Compile();

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
		}

		ImGui::SameLine(425 * fontScale * 0.75f);

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

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 250);

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
		ImGui::SameLine(ImGui::GetWindowWidth() - 190);

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
		ImGui::SameLine(ImGui::GetWindowWidth() - 115);

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