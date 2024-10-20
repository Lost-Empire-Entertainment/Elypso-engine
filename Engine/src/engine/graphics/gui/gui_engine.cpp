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

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		io.FontGlobalScale = stof(ConfigFile::GetValue("gui_fontScale"));

		isImguiInitialized = true;
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
			if (renderSceneWindow) Render::RenderToImguiWindow();

			Compilation::RenderBuildingWindow();

			if (renderUnsavedShutdownWindow) SaveBefore(SaveBeforeState::shutdown);
			if (renderUnsavedSceneSwitchWindow) SaveBefore(SaveBeforeState::sceneSwitch);

			bool firstUse = stoi(ConfigFile::GetValue("firstUse"));
			if (firstUse)
			{
				ConfigFile::SetValue("gui_firstTime", "1");
				ConfigFile::SetValue("firstUse", "0");
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
				if (assetPath == "")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Did not get path!\n");
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

			vec3 newPos = Render::camera.GetCameraPosition() + Render::camera.GetFront() * 5.0f;

			int resultX = static_cast<int>(newPos.x);
			int resultY = static_cast<int>(newPos.y);
			int resultZ = static_cast<int>(newPos.z);
			newPos = vec3(resultX, resultY, resultZ);

			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					string originPath = Engine::filesPath + "\\models\\cube.fbx";
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cube", "", true);
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\" + targetName + ".fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
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
						nextID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					string originPath = Engine::filesPath + "\\models\\sphere.fbx";
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Sphere", "", true);
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\" + targetName + ".fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
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
						nextID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					string originPath = Engine::filesPath + "\\models\\cylinder.fbx";
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cylinder", "", true);
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\" + targetName + ".fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
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
						nextID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Cone"))
				{
					string originPath = Engine::filesPath + "\\models\\cone.fbx";
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Cone", "", true);
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\" + targetName + ".fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
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
						nextID);

					SceneFile::SaveScene();
				}
				else if (ImGui::MenuItem("Pyramid"))
				{
					string originPath = Engine::filesPath + "\\models\\pyramid.fbx";
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Pyramid", "", true);
					string targetName = path(targetPath).stem().string();

					File::CreateNewFolder(targetPath);
					string destinationPath = targetPath + "\\" + targetName + ".fbx";
					File::CopyFileOrFolder(originPath, destinationPath);

					unsigned int nextID = GameObject::nextID++;

					Importer::Initialize(
						newPos,
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
						nextID);

					SceneFile::SaveScene();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light source"))
			{
				if (ImGui::MenuItem("Point light"))
				{
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Point light", "", true);
					string targetName = path(targetPath).stem().string();
					File::CreateNewFolder(targetPath);

					string filePath = targetPath + "\\" + targetName + ".txt";

					string name = "Billboard";
					unsigned int nextID = GameObject::nextID++;
					unsigned int nextID2 = GameObject::nextID++;

					shared_ptr<GameObject> obj = 
						PointLight::InitializePointLight(
							newPos,
							vec3(0),
							vec3(1),
							filePath,
							Engine::filesPath + "\\shaders\\Basic_model.vert",
							Engine::filesPath + "\\shaders\\Basic.frag",
							vec3(1),
							1.0f,
							1.0f,
							targetName,
							nextID,
							true,

							//billboard values
							Engine::filesPath + "\\shaders\\Basic_texture.vert",
							Engine::filesPath + "\\shaders\\Basic_texture.frag",
							Engine::filesPath + "\\icons\\pointLight.png",
							32,
							name,
							nextID2);

					SceneFile::SaveScene();
				}
				if (ImGui::MenuItem("Spotlight"))
				{
					string targetPath = File::AddIndex(Engine::currentGameobjectsPath, "Spotlight", "", true);
					string targetName = path(targetPath).stem().string();
					File::CreateNewFolder(targetPath);

					string filePath = targetPath + "\\" + targetName + ".txt";

					string name = "Billboard";
					unsigned int nextID = GameObject::nextID++;
					unsigned int nextID2 = GameObject::nextID++;

					shared_ptr<GameObject> obj = 
						SpotLight::InitializeSpotLight(
							newPos,
							vec3(0),
							vec3(1),
							filePath,
							Engine::filesPath + "\\shaders\\Basic_model.vert",
							Engine::filesPath + "\\shaders\\Basic.frag",
							vec3(1),
							1.0f,
							1.0f,
							12.5f,
							17.5f,
							targetName,
							nextID,
							true,

							//billboard values
							Engine::filesPath + "\\shaders\\Basic_texture.vert",
							Engine::filesPath + "\\shaders\\Basic_texture.frag",
							Engine::filesPath + "\\icons\\spotLight.png",
							32,
							name,
							nextID2);

					SceneFile::SaveScene();
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
						File::CreateNewFolder(targetPath);

						string filePath = targetPath + "\\" + targetName + ".txt";

						string name = "Billboard";
						unsigned int nextID = GameObject::nextID++;
						unsigned int nextID2 = GameObject::nextID++;

						shared_ptr<GameObject> obj =
							DirectionalLight::InitializeDirectionalLight(
								newPos,
								vec3(0),
								vec3(1),
								filePath,
								Engine::filesPath + "\\shaders\\Basic_model.vert",
								Engine::filesPath + "\\shaders\\Basic.frag",
								vec3(1),
								1.0f,
								targetName,
								nextID,
								true,

								//billboard values
								Engine::filesPath + "\\shaders\\Basic_texture.vert",
								Engine::filesPath + "\\shaders\\Basic_texture.frag",
								Engine::filesPath + "\\icons\\directionalLight.png",
								32,
								name,
								nextID2);

						SceneFile::SaveScene();
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

			if (ImGui::MenuItem("Welcome"))
			{
				ConfigFile::SetValue("gui_firstTime", "1");
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
				Compilation::installerType = Compilation::InstallerType::compile;
				Compilation::Compile();

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
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
				ImGui::EndMenu();
			}
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
						"Error: Failed to open link to Github repository issues page! " + string(e.what()) + "\n");
				}

				ImGui::CloseCurrentPopup();
				ImGui::EndMenu();
			}
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
		isImguiInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
#endif