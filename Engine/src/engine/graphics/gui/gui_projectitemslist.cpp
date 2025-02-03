//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <filesystem>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"

//engine
#include "gui_projectitemslist.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "sceneFile.hpp"
#include "texture.hpp"
#include "console.hpp"
#include "gui_engine.hpp"
#include "gui_settings.hpp"
#include "fileUtils.hpp"
#include "importer.hpp"
#include "audioplayercomponent.hpp"
#include "audio.hpp"
#include "meshcomponent.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"

using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;
using std::cout;
using std::filesystem::file_size;
using std::to_string;

using Graphics::Shape::GameObjectManager;
using Core::Engine;
using EngineFile::SceneFile;
using Graphics::Texture;
using Core::ConsoleManager;
using ConsoleCaller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;
using Utils::File;
using Graphics::Shape::Importer;
using Graphics::GUI::GUISettings;
using Graphics::Components::AudioPlayerComponent;
using Core::Audio;
using Graphics::Components::MeshComponent;
using Utils::String;
using Core::Select;

namespace Graphics::GUI
{
	void GUIProjectItemsList::RenderProjectItemsList()
	{
		ImVec2 windowSize = ImVec2(400.0f, 400.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize;

		string listType;
		if (type == Type::SkyboxTexture_left
			|| type == Type::SkyboxTexture_right
			|| type == Type::SkyboxTexture_top
			|| type == Type::SkyboxTexture_bottom
			|| type == Type::SkyboxTexture_front
			|| type == Type::SkyboxTexture_back)
		{
			listType = "Skybox texture";
		}
		else if (type == Type::GameobjectModel) listType = "Gameobject model";
		else if (type == Type::GameobjectTexture) listType = "Gameobject texture";
		else if (type == Type::Scene) listType = "Scene";
		else if (type == Type::Audio) listType = "Audio";

		if (renderProjectItemsList
			&& ImGui::Begin(listType.c_str(), NULL, windowFlags))
		{
			RenderProjectItemsListContent();
			ImGui::End();
		}

		if (renderLargeImportConfirm) RenderLargeInitializeConfirm();
	}

	void GUIProjectItemsList::RenderProjectItemsListContent()
	{
		if (!isContentVectorFilled)
		{
			content.clear();
			selectedPath = "";

			switch (type)
			{
			case Type::SkyboxTexture_right:
			case Type::SkyboxTexture_left:
			case Type::SkyboxTexture_top:
			case Type::SkyboxTexture_bottom:
			case Type::SkyboxTexture_front:
			case Type::SkyboxTexture_back:
			case Type::GameobjectTexture:
			{
				for (const auto& entry : directory_iterator(Engine::texturesPath))
				{
					if (is_regular_file(entry))
					{
						string extension = path(entry).extension().string();
						if (extension == ".png"
							|| extension == ".jpg"
							|| extension == ".jpeg")
						{
							content.push_back(entry.path().string());
						}
					}
				}
				break;
			}
			case Type::GameobjectModel:
			{
				string modelsFolder = (path(Engine::projectPath) / "models").string();
				for (const auto& entry : directory_iterator(modelsFolder))
				{
					if (is_regular_file(entry))
					{
						string extension = path(entry).extension().string();
						if (extension == ".fbx"
							|| extension == ".obj"
							|| extension == ".gltf")
						{
							content.push_back(entry.path().string());
						}
					}
				}
				break;
			}
			case Type::Scene:
			{
				for (const auto& entry : directory_iterator(path(Engine::scenePath).parent_path().parent_path().string()))
				{
					if (is_directory(entry))
					{
						for (const auto& child : directory_iterator(entry))
						{
							if (is_regular_file(child)
								&& child.path().filename().string() == "scene.txt")
							{
								content.push_back(entry.path().string());
								break;
							}
						}
					}
				}
				break;
			}
			case Type::Audio:
			{
				string modelsFolder = (path(Engine::projectPath) / "audio").string();
				for (const auto& entry : directory_iterator(modelsFolder))
				{
					if (is_regular_file(entry))
					{
						string extension = path(entry).extension().string();
						if (extension == ".mp3"
							|| extension == ".flac"
							|| extension == ".wav")
						{
							content.push_back(entry.path().string());
						}
					}
				}
				break;
			}
			}

			isContentVectorFilled = true;
		}

		ImVec2 childSize = ImVec2(
			ImGui::GetWindowWidth() - 10,
			ImGui::GetWindowHeight() - 100);
		if (ImGui::BeginChild("##content", childSize))
		{
			static string chosenEntry = "";

			for (const auto& entry : content)
			{
				string name = path(entry).stem().string();

				bool isChosen = (chosenEntry == name);

				if (isChosen)
				{
					ImVec4 color = ImVec4(1.0f, 1.0f, 0.6f, 1.0f);
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				}

				if (ImGui::Selectable(name.c_str()))
				{
					chosenEntry = name;

					switch (type)
					{
					case Type::SkyboxTexture_right:
					case Type::SkyboxTexture_left:
					case Type::SkyboxTexture_top:
					case Type::SkyboxTexture_bottom:
					case Type::SkyboxTexture_front:
					case Type::SkyboxTexture_back:
					case Type::GameobjectTexture:
					case Type::GameobjectModel:
					case Type::Audio:
					{
						selectedPath = entry;
						break;
					}
					case Type::Scene:
					{
						for (const auto& child : directory_iterator(entry))
						{
							if (is_regular_file(child)
								&& child.path().filename().string() == "scene.txt")
							{
								selectedPath = child.path().string();
								break;
							}
						}
						break;
					}
					}
				}

				if (isChosen)
				{
					ImGui::PopStyleColor();
				}
			}
		}
		ImGui::EndChild();

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Select", buttonSize)
			&& selectedPath != "")
		{
			switch (type)
			{
			case Type::SkyboxTexture_right:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();

				GUISettings::skyboxTextures["right"] = texPath;
				break;
			}
			case Type::SkyboxTexture_left:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();
				GUISettings::skyboxTextures["left"] = texPath;
				break;
			}
			case Type::SkyboxTexture_top:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();
				GUISettings::skyboxTextures["top"] = texPath;
				break;
			}
			case Type::SkyboxTexture_bottom:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();
				GUISettings::skyboxTextures["bottom"] = texPath;
				break;
			}
			case Type::SkyboxTexture_front:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();
				GUISettings::skyboxTextures["front"] = texPath;
				break;
			}
			case Type::SkyboxTexture_back:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
				string texPath = (path("textures") / fileAndExtension).string();
				GUISettings::skyboxTextures["back"] = texPath;
				break;
			}
			case Type::GameobjectTexture:
			{
				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();

				string originPath = (path(Engine::projectPath) / "textures" / fileAndExtension).string();
				string targetPath = (path(Engine::projectPath) / path(obj->GetTxtFilePath()).parent_path().string() / fileAndExtension).string();
				
				File::CopyFileOrFolder(originPath, targetPath);

				Texture::LoadTexture(obj, targetPath, textureType, true);
				break;
			}
			case Type::GameobjectModel:
			{
				ModelImportCheck();
				break;
			}
			case Type::Scene:
			{
				if (!selectStartScene)
				{
					if (Engine::scenePath == selectedPath)
					{
						ConsoleManager::WriteConsoleMessage(
							ConsoleCaller::FILE,
							ConsoleType::EXCEPTION,
							"Error: Cannot switch to scene '" + path(selectedPath).parent_path().stem().string() + "' because it is already open!\n");
					}
					else SceneFile::LoadScene(selectedPath);
				}
				else
				{
					Engine::gameFirstScene = path(selectedPath).parent_path().stem().string();

					ConsoleManager::WriteConsoleMessage(
						ConsoleCaller::INPUT,
						ConsoleType::INFO,
						"Set game first scene to " + Engine::gameFirstScene + "\n");
				}

				break;
			}
			case Type::Audio:
			{
				auto audioPlayerComponent = obj->GetComponent<AudioPlayerComponent>();
				string audioFileName = path(selectedPath).filename().string();
				audioPlayerComponent->SetName(audioFileName);

				ConsoleManager::WriteConsoleMessage(
					ConsoleCaller::INPUT,
					ConsoleType::INFO,
					"Assigned audio file '" + audioFileName + "' to '" + obj->GetName() + "'.\n");
			}
			}

			obj = nullptr;

			isContentVectorFilled = false;
			renderProjectItemsList = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			obj = nullptr;

			isContentVectorFilled = false;
			renderProjectItemsList = false;
		}
	}

	void GUIProjectItemsList::RenderLargeInitializeConfirm()
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


		string title = "WARNING: LARGE MODEL";
		ImGui::Begin(title.c_str(), nullptr, flags);

		string description = "Selected model '" + name + "' size is over " + to_string(static_cast<int>(size)) + "MB! Elypso engine may freeze or fail to import the model - proceed with import? The scene will be automatically saved before importing starts.";
		//text padding on both sides
		float sidePadding = 20.0f;
		//calculate available width for text after padding
		float textWidth = windowSize.x - (sidePadding * 2);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + sidePadding);
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + textWidth);
		ImGui::TextWrapped("%s", description.c_str());
		ImGui::PopTextWrapPos();

		ImVec2 currentWindowPos = ImGui::GetWindowPos();

		ImVec2 windowCenter(
			currentWindowPos.x + windowSize.x * 0.5f,
			currentWindowPos.y + windowSize.y * 0.5f);

		ImVec2 buttonSize(120, 50);

		ImVec2 button1Pos(
			windowSize.x * 0.4f - buttonSize.x,
			(ImGui::GetWindowHeight() / 1.5f) - (buttonSize.y / 2));
		ImVec2 button2Pos(
			windowSize.x * 0.6f,
			(ImGui::GetWindowHeight() / 1.5f) - (buttonSize.y / 2));

		ImGui::SetCursorPos(button1Pos);
		if (ImGui::Button("Import", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				ConsoleCaller::INPUT,
				ConsoleType::INFO,
				"Confirmed large model '" + name + "' import.\n");

			SceneFile::SaveScene();

			Initialize();
			renderLargeImportConfirm = false;
		}

		ImGui::SetCursorPos(button2Pos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ConsoleManager::WriteConsoleMessage(
				ConsoleCaller::INPUT,
				ConsoleType::INFO,
				"Cancelled large model '" + name + "' import.\n");
			renderLargeImportConfirm = false;
		}

		ImGui::End();
	}

	void GUIProjectItemsList::ModelImportCheck()
	{
		bool confirmInitialize = false;
		size_t originalSize = file_size(selectedPath);
		name = path(selectedPath).stem().string();
		size = static_cast<double>(originalSize) / (1024 * 1024);
		double fileSizeLimit = 100; //100MB to trigger file size warning
		if (size > fileSizeLimit)
		{
			cout << "!!!! dont initialize, size is " + to_string(size) + "MB...\n";
			renderLargeImportConfirm = true;
		}
		else
		{
			cout << "!!!! initialize, size is " + to_string(size) + "MB...\n";
			Initialize();
		}
	}

	void GUIProjectItemsList::Initialize()
	{
		obj = Select::selectedObj;

		//general data
		vec3 pos = obj->GetTransform()->GetPosition();
		vec3 rot = obj->GetTransform()->GetRotation();
		vec3 scale = obj->GetTransform()->GetScale();
		string name = obj->GetName();
		unsigned int ID = obj->GetID();

		//audio data
		bool is3D{};
		float maxRange{};
		float minRange{};
		float volume{};
		string audioFilePath{};
		auto apc = obj->GetComponent<AudioPlayerComponent>();
		if (apc)
		{
			is3D = apc->Is3D();
			maxRange = apc->GetMaxRange();
			minRange = apc->GetMinRange();
			volume = apc->GetVolume();
			audioFilePath = apc->GetName();
		}

		//store model origin and target path
		string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();
		string originPath = (path(Engine::projectPath) / "models" / fileAndExtension).string();
		string targetFolderName{};
		for (const auto& entry : directory_iterator(Engine::currentGameobjectsPath))
		{
			string pathName = path(entry).stem().string();
			if (name == pathName)
			{
				targetFolderName = path(entry).stem().string();
				break;
			}
		}
		string targetPath = (path(Engine::currentGameobjectsPath) / targetFolderName / fileAndExtension).string();

		//delete old gameobject and its model file
		GameObjectManager::DestroyGameObject(obj, false);
		//then create new folder
		File::CreateNewFolder(path(targetPath).parent_path().string());
		//then copy model from origin to target path
		File::CopyFileOrFolder(originPath, targetPath);

		//and finally rename model to correct name
		string newCorrectFolder = path(targetPath).parent_path().string();
		string nameAndExtension = targetFolderName + path(targetPath).extension().string();
		string newCorrectPath = (path(newCorrectFolder) / nameAndExtension).string();
		File::MoveOrRenameFileOrFolder(targetPath, newCorrectPath, true);

		Importer::Initialize(
			pos,
			rot,
			scale,
			newCorrectPath,
			"DEFAULTDIFF",
			"DEFAULTSPEC",
			"EMPTY",
			"EMPTY",
			false,
			1.0f,
			name,
			ID);

		if (audioFilePath != "")
		{
			for (const auto& obj : GameObjectManager::GetObjects())
			{
				if (obj->GetName() == name)
				{
					shared_ptr<GameObject> initializedObj = obj;
					auto apc = initializedObj->AddComponent<AudioPlayerComponent>();

					apc->Set3DState(is3D);
					apc->SetMaxRange(maxRange);
					apc->SetMinRange(minRange);
					apc->SetVolume(volume);
					apc->SetName(audioFilePath);

					break;
				}
			}
		}
	}
}
#endif