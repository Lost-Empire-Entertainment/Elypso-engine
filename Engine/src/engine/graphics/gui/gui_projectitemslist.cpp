//Copyright(C) 2024 Lost Empire Entertainment
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
#include "gui_settings.hpp"

using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;
using std::cout;

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

		if (renderProjectItemsList
			&& ImGui::Begin(listType.c_str(), NULL, windowFlags))
		{
			RenderProjectItemsListContent();

			ImGui::End();
		}
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
				string parentFolder = path(obj->GetTxtFilePath()).parent_path().string();
				string modelFileName = obj->GetName();

				string finalPath;

				string modelFBX = modelFileName + ".fbx";
				string fbxPath = (path(Engine::projectPath) / parentFolder / modelFBX).string();

				string modelOBJ = modelFileName + ".obj";
				string objPath = (path(Engine::projectPath) / parentFolder / modelOBJ).string();

				string modelGLTF = modelFileName + ".gltf";
				string gltfPath = (path(Engine::projectPath) / parentFolder / modelGLTF).string();

				if (exists(fbxPath)) finalPath = fbxPath;
				else if (exists(objPath)) finalPath = objPath;
				else if (exists(gltfPath)) finalPath = gltfPath;
				if (!finalPath.empty()) File::DeleteFileOrfolder(finalPath);

				string fileAndExtension = path(selectedPath).stem().string() + path(selectedPath).extension().string();

				string originPath = (path(Engine::projectPath) / "models" / fileAndExtension).string();
				string targetPath = (path(Engine::projectPath) / path(obj->GetTxtFilePath()).parent_path().string() / fileAndExtension).string();

				File::CopyFileOrFolder(originPath, targetPath);

				string nameAndExtension = modelFileName + (path(targetPath).extension()).string();
				string newTargetPath = (path(targetPath).parent_path() / nameAndExtension).string();
				File::MoveOrRenameFileOrFolder(targetPath, newTargetPath, true);

				string objName = obj->GetName();
				unsigned int objID = obj->GetID();
				string objTxtPath = (path(Engine::projectPath) / obj->GetTxtFilePath()).string();
				vec3 objPos = obj->GetTransform()->GetPosition();
				vec3 objRot = obj->GetTransform()->GetRotation();
				vec3 objScale = obj->GetTransform()->GetScale();
				
				GameObjectManager::DestroyGameObject(obj, true);

				Importer::Initialize(
					objPos,
					objRot,
					objScale,
					newTargetPath,
					"DEFAULTDIFF",
					"DEFAULTSPEC",
					"EMPTY",
					"EMPTY",
					objName,
					objID);
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
}
#endif