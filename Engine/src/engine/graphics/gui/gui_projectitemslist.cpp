//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <filesystem>

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
#include "gui.hpp"
#include "gui_settings.hpp"

using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;

using Graphics::Shape::GameObjectManager;
using Core::Engine;
using EngineFile::SceneFile;
using Graphics::Texture;
using Core::ConsoleManager;
using ConsoleCaller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;

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
		if (type == Type::GameobjectTexture) listType = "Gameobject texture";
		if (type == Type::Scene) listType = "Scene";

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
				SceneFile::skyboxTexturesMap["right"] = selectedPath;
				break;
			}
			case Type::SkyboxTexture_left:
			{
				SceneFile::skyboxTexturesMap["left"] = selectedPath;
				break;
			}
			case Type::SkyboxTexture_top:
			{
				SceneFile::skyboxTexturesMap["top"] = selectedPath;
				break;
			}
			case Type::SkyboxTexture_bottom:
			{
				SceneFile::skyboxTexturesMap["bottom"] = selectedPath;
				break;
			}
			case Type::SkyboxTexture_front:
			{
				SceneFile::skyboxTexturesMap["front"] = selectedPath;
				break;
			}
			case Type::SkyboxTexture_back:
			{
				SceneFile::skyboxTexturesMap["back"] = selectedPath;
				break;
			}
			case Type::GameobjectTexture:
			{
				Texture::LoadTexture(obj, selectedPath, textureType, true);
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
							"Cannot switch to scene '" + path(selectedPath).parent_path().stem().string() + "' because it is already open!\n");
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