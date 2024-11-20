//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <string>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui_engine.hpp"
#include "gui_console.hpp"
#include "gui_settings.hpp"
#include "gui_inspector.hpp"
#include "gui_importAsset.hpp"
#include "gui_scenehierarchy.hpp"
#include "gui_projecthierarchy.hpp"
#include "gui_createscene.hpp"
#include "gui_rename.hpp"
#include "gui_credits.hpp"
#include "gui_links.hpp"
#include "gui_projectitemslist.hpp"
#include "gui_firstTime.hpp"
#include "gui_scenewindow.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "browserUtils.hpp"
#include "gameobject.hpp"
#include "timeManager.hpp"
#include "importer.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "directionallight.hpp"
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
using Core::Select;
using Utils::File;
using Utils::Browser;
using Utils::String;
using Graphics::Shape::Importer;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Graphics::Shape::DirectionalLight;
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
		string imguiConfigFile = (path(Engine::docsPath) / "imgui.ini").string();
		string imguiTemplateFile = (path(Engine::filesPath) / "imgui.ini").string();
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

		static string tempString = (path(Engine::docsPath) / "imgui.ini").string();
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF(((path(Engine::filesPath) / "fonts" / "coda" / "Coda-Regular.ttf").string()).c_str(), 16.0f);

		SetStyle();

		isImguiInitialized = true;
	}

	void EngineGUI::SetStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = stof(ConfigFile::GetValue("gui_fontScale"));

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 4.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 4.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 4.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 4.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 4.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 20.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.1630901098251343f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.08627451211214066f, 0.08627451211214066f, 0.08627451211214066f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.105882354080677f, 0.105882354080677f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8784313797950745f, 0.8784313797950745f, 0.8784313797950745f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9803921580314636f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.1490196138620377f, 0.1490196138620377f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2470588237047195f, 0.2470588237047195f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3294117748737335f, 0.3294117748737335f, 0.3294117748737335f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.3098039329051971f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 0.7803921699523926f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 0.6705882549285889f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.9490196108818054f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.2235294133424759f, 0.2235294133424759f, 0.2235294133424759f, 0.8627451062202454f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.321568638086319f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 0.9725490212440491f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4235294163227081f, 0.4235294163227081f, 0.4235294163227081f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
	}

	ImVec2 EngineGUI::CenterWindow(const ImVec2& size)
	{
		int intWidth, intHeight;
		glfwGetFramebufferSize(Render::window, &intWidth, &intHeight);

		float glfwWindowWidth = static_cast<float>(intWidth);
		float glfwWindowHeight = static_cast<float>(intHeight);

		float posX = (glfwWindowWidth - size.x) / 2.0f;
		float posY = (glfwWindowHeight - size.y) / 2.0f;

		return ImVec2(posX, posY);
	}

	void EngineGUI::Render()
	{
		if (isImguiInitialized)
		{
			if (!Engine::IsUserIdle())
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
					GUIConsole::RenderConsole();
					GUISettings::RenderSettings();
					GUIInspector::RenderInspector();
					GUIImportAsset::RenderImportAsset();
					GUISceneHierarchy::RenderSceneHierarchy();
					GUIProjectHierarchy::RenderProjectHierarchy();
					GUICreateScene::RenderCreateSceneWindow();
					GUIRename::RenderRenameWindow();
					GUICredits::RenderCreditsWindow();
					GUILinks::RenderLinksWindow();
					GUIProjectItemsList::RenderProjectItemsList();
					GUIFirstTime::RenderFirstTime();
				}

				bool renderSceneWindow = stoi(ConfigFile::GetValue("gui_sceneWindow"));
				if (renderSceneWindow) GUISceneWindow::RenderSceneWindow();

				Compilation::RenderBuildingWindow();

				if (renderUnsavedShutdownWindow) SaveBefore(SaveBeforeState::shutdown);
				if (renderUnsavedSceneSwitchWindow) SaveBefore(SaveBeforeState::sceneSwitch);

				/*
				* 
				* DISABLED FOR NOW
				* WILL BE UPDATED IN A FUTURE VERSION
				* 
				bool firstUse = stoi(ConfigFile::GetValue("firstUse"));
				if (firstUse)
				{
					ConfigFile::SetValue("gui_firstTime", "1");
					ConfigFile::SetValue("firstUse", "0");
				}
				*/

				ImGui::Render();
			}

			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData != nullptr)
			{
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
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
				GUIProjectItemsList::type = GUIProjectItemsList::Type::Scene;
				GUIProjectItemsList::selectStartScene = false;
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
				if (assetPath != "")
				{
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
							"Error: File '" + name + "' with extension '" + extension + "' is not yet supported!");
					}
					else
					{
						bool foundExisting = false;
						string existingFilePath;
						string existingFileName;
						string importedFileName = path(assetPath).stem().string();
						for (const auto& file : directory_iterator(Engine::currentGameobjectsPath))
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
								"Error: Gameobject '" + importedFileName + "' already exists in the current scene '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");
						}
						else
						{
							GUIImportAsset::renderImportAsset = true;
							GUIImportAsset::assetPath = assetPath;
						}
					}
				}
			}

			vec3 newPos = Render::camera.GetCameraPosition() + Render::camera.GetFront() * 5.0f;

			int resultX = static_cast<int>(newPos.x);
			int resultY = static_cast<int>(newPos.y);
			int resultZ = static_cast<int>(newPos.z);
			newPos = vec3(resultX, resultY, resultZ);

			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					string originPath = (path(Engine::filesPath) / "models" / "cube.fbx").string();
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cube", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".fbx";

					File::CreateNewFolder(targetPath);
					string destinationPath = (path(targetPath) / targetNameAndExtension).string();
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
						vec3(0),
						vec3(1),
						destinationPath,
						(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
						(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						nextID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					string originPath = (path(Engine::filesPath) / "models" / "sphere.fbx").string();
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Sphere", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".fbx";

					File::CreateNewFolder(targetPath);
					string destinationPath = (path(targetPath) / targetNameAndExtension).string();
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
						vec3(0),
						vec3(1),
						destinationPath,
						(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
						(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						nextID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					string originPath = (path(Engine::filesPath) / "models" / "cylinder.fbx").string();
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cylinder", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".fbx";

					File::CreateNewFolder(targetPath);
					string destinationPath = (path(targetPath) / targetNameAndExtension).string();
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
						vec3(0),
						vec3(1),
						destinationPath,
						(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
						(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						nextID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Cone"))
				{
					string originPath = (path(Engine::filesPath) / "models" / "cone.fbx").string();
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cone", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".fbx";

					File::CreateNewFolder(targetPath);
					string destinationPath = (path(targetPath) / targetNameAndExtension).string();
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
						vec3(0),
						vec3(1),
						destinationPath,
						(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
						(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						nextID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				else if (ImGui::MenuItem("Pyramid"))
				{
					string originPath = (path(Engine::filesPath) / "models" / "pyramid.fbx").string();
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Pyramid", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".fbx";

					File::CreateNewFolder(targetPath);
					string destinationPath = (path(targetPath) / targetNameAndExtension).string();
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
						vec3(0),
						vec3(1),
						destinationPath,
						(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
						(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
						"DEFAULTDIFF",
						"DEFAULTSPEC",
						"EMPTY",
						"EMPTY",
						32,
						targetName,
						nextID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light source"))
			{
				if (ImGui::MenuItem("Point light"))
				{
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Point light", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".txt";
					File::CreateNewFolder(targetPath);

					string filePath = (path(targetPath) / targetNameAndExtension).string();

					string name = "Billboard";
					unsigned int nextID = GameObject::nextID++;
					unsigned int nextID2 = GameObject::nextID++;

					shared_ptr<GameObject> obj = 
						PointLight::InitializePointLight(
							newPos,
							vec3(0),
							vec3(1),
							filePath,
							(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
							(path(Engine::filesPath) / "shaders" / "Basic.frag").string(),
							vec3(1),
							1.0f,
							1.0f,
							targetName,
							nextID,
							true,
							true,

							//billboard values
							(path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string(),
							(path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string(),
							(path(Engine::filesPath) / "icons" / "pointLight.png").string(),
							32,
							name,
							nextID2,
							true);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				if (ImGui::MenuItem("Spotlight"))
				{
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Spotlight", "", true);
					string targetName = path(targetPath).stem().string();
					string targetNameAndExtension = targetName + ".txt";
					File::CreateNewFolder(targetPath);

					string filePath = (path(targetPath) / targetNameAndExtension).string();

					string name = "Billboard";
					unsigned int nextID = GameObject::nextID++;
					unsigned int nextID2 = GameObject::nextID++;

					shared_ptr<GameObject> obj = 
						SpotLight::InitializeSpotLight(
							newPos,
							vec3(0),
							vec3(1),
							filePath,
							(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
							(path(Engine::filesPath) / "shaders" / "Basic.frag").string(),
							vec3(1),
							1.0f,
							1.0f,
							12.5f,
							17.5f,
							targetName,
							nextID,
							true,
							true,

							//billboard values
							(path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string(),
							(path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string(),
							(path(Engine::filesPath) / "icons" / "spotLight.png").string(),
							32,
							name,
							nextID2,
							true);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				if (ImGui::MenuItem("Directional light"))
				{
					if (GameObjectManager::GetDirectionalLight() != nullptr)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::EXCEPTION,
							"Error: Cannot have more than one directional light in scene '" + path(Engine::scenePath).parent_path().stem().string() + "'!");
					}
					else
					{
						string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Directional light", "", true);
						string targetName = path(targetPath).stem().string();
						string targetNameAndExtension = targetName + ".txt";
						File::CreateNewFolder(targetPath);

						string filePath = (path(targetPath) / targetNameAndExtension).string();

						string name = "Billboard";
						unsigned int nextID = GameObject::nextID++;
						unsigned int nextID2 = GameObject::nextID++;

						shared_ptr<GameObject> obj =
							DirectionalLight::InitializeDirectionalLight(
								newPos,
								vec3(0),
								vec3(1),
								filePath,
								(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
								(path(Engine::filesPath) / "shaders" / "Basic.frag").string(),
								vec3(1),
								1.0f,
								targetName,
								nextID,
								true,
								true,

								//billboard values
								(path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string(),
								(path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string(),
								(path(Engine::filesPath) / "icons" / "directionalLight.png").string(),
								32,
								name,
								nextID2,
								true);

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
			if (ImGui::MenuItem("Scene window"))
			{
				ConfigFile::SetValue("gui_sceneWindow", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

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

			if (ImGui::MenuItem("Console"))
			{
				ConfigFile::SetValue("gui_console", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			/*
			* 
			* DISABLED FOR NOW
			* WILL BE UPDATED IN A FUTURE VERSION
			* 
			if (ImGui::MenuItem("Welcome"))
			{
				ConfigFile::SetValue("gui_firstTime", "1");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			*/

			ImGui::EndMenu();
		}

		ImGui::SameLine(170 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::IsItemClicked())
			{
				GUISettings::renderSettings = true;

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}

		ImGui::SameLine(240 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Compile"))
		{
			if (ImGui::IsItemClicked())
			{
				Compilation::installerType = Compilation::InstallerType::compile;
				Compilation::Compile();

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Shortcut: Ctrl + B");
			ImGui::EndTooltip();
		}

		ImGui::SameLine(310 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Run"))
		{
			if (ImGui::IsItemClicked())
			{
				Compilation::Run();

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Shortcut: F5");
			ImGui::EndTooltip();
		}

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 225 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Links"))
		{
			if (ImGui::IsItemClicked())
			{
				GUILinks::renderLinksWindow = true;

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}

		//on the right side
		ImGui::SameLine(ImGui::GetWindowWidth() - 175 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Credits"))
		{
			if (ImGui::IsItemClicked())
			{
				GUICredits::renderCreditsWindow = true;

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
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
						"Error: Failed to open link to Github repository issues page! " + string(e.what()) + "\n");
				}

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void EngineGUI::SaveBefore(SaveBeforeState saveBeforeState)
	{
		ImVec2 windowSize = ImVec2(500.0f, 300.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings;

		string title = "You have unsaved changes. Save before closing?";
		ImGui::Begin(title.c_str(), nullptr, flags);

		ImVec2 currentWindowPos = ImGui::GetWindowPos();

		ImVec2 windowCenter(
			currentWindowPos.x + windowSize.x * 0.5f, 
			currentWindowPos.y + windowSize.y * 0.5f);

		ImVec2 buttonSize(120, 50);

		ImVec2 button1Pos(
			windowSize.x * 0.3f - buttonSize.x,
			(ImGui::GetWindowHeight() / 2) - (buttonSize.y / 2));
		ImVec2 button2Pos(
			windowSize.x * 0.625f - buttonSize.x,
			(ImGui::GetWindowHeight() / 2) - (buttonSize.y / 2));
		ImVec2 button3Pos(
			windowSize.x * 0.7f,
			(ImGui::GetWindowHeight() / 2) - (buttonSize.y / 2));

		if (saveBeforeState == SaveBeforeState::shutdown)
		{
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
		}
		else if (saveBeforeState == SaveBeforeState::sceneSwitch)
		{
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
		}

		ImGui::End();
	}

	void EngineGUI::Shutdown()
	{
		if (isImguiInitialized)
		{
			isImguiInitialized = false;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}
}
#endif