//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <cmath>
#include <filesystem>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"
#include "type_ptr.hpp"
#include "glm.hpp"

//engine
#include "gui_settings.hpp"
#include "gui_engine.hpp"
#include "gui_console.hpp"
#include "render.hpp"
#include "input.hpp"
#include "console.hpp"
#include "grid.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "fileexplorer.hpp"
#include "configFile.hpp"
#include "stringUtils.hpp"
#include "gui_projectitemslist.hpp"
#include "skybox.hpp"

using std::to_string;
using std::stof;
using std::round;
using glm::value_ptr;
using std::exception;
using std::filesystem::path;
using std::cout;

using EngineFile::SceneFile;
using Graphics::GUI::GUIConsole;
using Graphics::Render;
using Graphics::Grid;
using Core::Input;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using EngineFile::FileExplorer;
using EngineFile::ConfigFile;
using Utils::String;
using Graphics::Shape::Skybox;

namespace Graphics::GUI
{
	void GUISettings::RenderSettings()
	{
		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoSavedSettings;

		if (renderSettings
			&& ImGui::Begin("Settings", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderSettings = false;
			}

			if (ImGui::BeginTabBar("Settings"))
			{
				if (ImGui::BeginTabItem("Input"))
				{
					InputSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("GUI style"))
				{
					GUIStyleSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Graphics"))
				{
					GraphicsSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Other"))
				{
					OtherSettings();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}
	void GUISettings::InputSettings()
	{
		ImGui::Text("Forwards: W");
		ImGui::Text("Backwards: S");
		ImGui::Text("Left: A");
		ImGui::Text("Right: D");
		ImGui::Text("Up: Space");
		ImGui::Text("Down: Left Control");
		ImGui::Text("Sprint: Left Shift");
		ImGui::Text("Enable camera movement: Hold right mouse button");
		ImGui::Text("Compile game: Ctrl + B");
		ImGui::Text("Run compiled game: Ctrl + R");
		ImGui::Text("Select GameObject: Left Mouse Button");
		ImGui::Text("Delete selected GameObject or node: Delete");
		//ImGui::Text("Copy selected object: Ctrl + C"); TEMPORARILY DISABLED, WILL BE FIXED IN A FUTURE VERSION
		//ImGui::Text("Paste copied object: Ctrl + V"); TEMPORARILY DISABLED, WILL BE FIXED IN A FUTURE VERSION
		ImGui::Text("Switch to X axis: X");
		ImGui::Text("Switch to Y axis: Y");
		ImGui::Text("Switch to Z axis: Z");
		ImGui::Text("Move selected object:");
		ImGui::Text("Press W and scroll up to increase or down to decrease value");
		ImGui::Text("Rotate selected object:");
		ImGui::Text("Press E and scroll up to increase or down to decrease value");
		ImGui::Text("Scale selected object:");
		ImGui::Text("Press R and scroll up to increase or down to decrease value");
	}

	void GUISettings::GUIStyleSettings()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		float fontScale = stof(ConfigFile::GetValue("gui_fontScale"));
		ImGui::Text("Font scale");

		if (ImGui::DragFloat("##fontScale", &fontScale, 0.01f, 0.1f, 2.0f))
		{
			if (fontScale < 1.0f) fontScale = 1.0f;
			if (fontScale > 2.0f) fontScale = 2.0f;

			ConfigFile::SetValue("gui_fontScale", to_string(fontScale));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fontScale"))
		{
			ConfigFile::SetValue("gui_fontScale", "1.5");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}

	void GUISettings::GraphicsSettings()
	{
		if (skyboxTextures.empty())
		{
			skyboxTextures["right"] = SceneFile::skyboxTexturesMap["right"];
			skyboxTextures["left"] = SceneFile::skyboxTexturesMap["left"];
			skyboxTextures["top"] = SceneFile::skyboxTexturesMap["top"];
			skyboxTextures["bottom"] = SceneFile::skyboxTexturesMap["bottom"];
			skyboxTextures["front"] = SceneFile::skyboxTexturesMap["front"];
			skyboxTextures["back"] = SceneFile::skyboxTexturesMap["back"];
		}

		ImGui::Text("Skybox textures");

		ImGui::Spacing();

		if (ImGui::Button("Right"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_right;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string rightName = path(skyboxTextures["right"]).filename().string();
		ImGui::Text("%s", rightName.c_str());

		if (ImGui::Button("Left"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_left;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string leftName = path(skyboxTextures["left"]).filename().string();
		ImGui::Text("%s", leftName.c_str());

		if (ImGui::Button("Top"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_top;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string topName = path(skyboxTextures["top"]).filename().string();
		ImGui::Text("%s", topName.c_str());

		if (ImGui::Button("Bottom"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_bottom;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string bottomName = path(skyboxTextures["bottom"]).filename().string();
		ImGui::Text("%s", bottomName.c_str());

		if (ImGui::Button("Front"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_front;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string frontName = path(skyboxTextures["front"]).filename().string();
		ImGui::Text("%s", frontName.c_str());

		if (ImGui::Button("Back"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::SkyboxTexture_back;
			GUIProjectItemsList::selectSkyboxTexture = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
		ImGui::SameLine();
		string backName = path(skyboxTextures["back"]).filename().string();
		ImGui::Text("%s", backName.c_str());

		if (ImGui::Button("Apply"))
		{
			SceneFile::skyboxTexturesMap["right"] = skyboxTextures["right"];
			SceneFile::skyboxTexturesMap["left"] = skyboxTextures["left"];
			SceneFile::skyboxTexturesMap["top"] = skyboxTextures["top"];
			SceneFile::skyboxTexturesMap["bottom"] = skyboxTextures["bottom"];
			SceneFile::skyboxTexturesMap["front"] = skyboxTextures["front"];
			SceneFile::skyboxTexturesMap["back"] = skyboxTextures["back"];

			vector<string> appliedSkyboxTextures
			{
				SceneFile::skyboxTexturesMap["right"],
				SceneFile::skyboxTexturesMap["left"],
				SceneFile::skyboxTexturesMap["top"],
				SceneFile::skyboxTexturesMap["bottom"],
				SceneFile::skyboxTexturesMap["front"],
				SceneFile::skyboxTexturesMap["back"]
			};
			Skybox::AssignSkyboxTextures(appliedSkyboxTextures, false);

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			SceneFile::skyboxTexturesMap["right"] = (path("textures") / "skybox_default.png").string();
			SceneFile::skyboxTexturesMap["left"] = (path("textures") / "skybox_default.png").string();
			SceneFile::skyboxTexturesMap["top"] = (path("textures") / "skybox_default.png").string();
			SceneFile::skyboxTexturesMap["bottom"] = (path("textures") / "skybox_default.png").string();
			SceneFile::skyboxTexturesMap["front"] = (path("textures") / "skybox_default.png").string();
			SceneFile::skyboxTexturesMap["back"] = (path("textures") / "skybox_default.png").string();

			vector<string> appliedSkyboxTextures
			{
				SceneFile::skyboxTexturesMap["right"],
				SceneFile::skyboxTexturesMap["left"],
				SceneFile::skyboxTexturesMap["top"],
				SceneFile::skyboxTexturesMap["bottom"],
				SceneFile::skyboxTexturesMap["front"],
				SceneFile::skyboxTexturesMap["back"]
			};
			Skybox::AssignSkyboxTextures(appliedSkyboxTextures, false);

			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::INFO,
				"Successfully reset skybox textures to default values!");

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}

	void GUISettings::OtherSettings()
	{
		string gameName = ConfigFile::GetValue("gameName");

		ImGui::Text("Set game name");
		if (gameName == "") ConfigFile::SetValue("gameName", "Game");
#ifdef _WIN32
		strcpy_s(gameNameChar, bufferSize, gameName.c_str());
#elif __linux__
		strncpy(gameNameChar, gameName.c_str(), bufferSize);
#endif
		if (ImGui::InputText("##objName", gameNameChar, bufferSize))
		{
			ConfigFile::SetValue("gameName", gameNameChar);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", Engine::gameExePath.c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("Apply"))
		{
			bool canApply = true;

			if (gameName == "Elypso engine"
				|| gameName == "Elypso hub"
				|| gameName == "Compiler")
			{
				nameDenyReason = NameDenyReason::invalidName;
				canApply = false;
			}

			if (canApply)
			{
				for (char c : gameName)
				{
					if (!String::IsValidSymbolInPath(c))
					{
						nameDenyReason = NameDenyReason::invalidCharacter;
						canApply = false;

						break;
					}
				}
			}

			if (!canApply)
			{
				switch (nameDenyReason)
				{
				case NameDenyReason::invalidCharacter:
					ConsoleManager::WriteConsoleMessage(
						Caller::INPUT,
						Type::EXCEPTION,
						"Error: Invalid character detected in game name! Please pick another game name.");

					Engine::gameExePath = (path(Engine::gameParentPath) / "Game.exe").string();
					ConfigFile::SetValue("gameName", "Game");
					ConfigFile::SaveConfigFile();
					SceneFile::SaveScene();

					break;

				case NameDenyReason::invalidName:
					ConsoleManager::WriteConsoleMessage(
						Caller::INPUT,
						Type::EXCEPTION,
						"Error: Name '" + gameName + "' is not allowed to be set as the game name! Please pick another name.\n");

					Engine::gameExePath = (path(Engine::gameParentPath) / "Game.exe").string();
					ConfigFile::SetValue("gameName", "Game");
					ConfigFile::SaveConfigFile();
					SceneFile::SaveScene();

					break;
				}
			}
			else
			{
				string gameNameAndExtension = gameName + ".exe";
				string finalPath = (path(Engine::gameParentPath) / gameNameAndExtension).string();
				Engine::gameExePath = finalPath;

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"Successfully set game name to '" + gameName + "' and game path as '" + Engine::gameExePath + "'!\n");

				ConfigFile::SaveConfigFile();
				SceneFile::SaveScene();
			}
		}

		ImGui::Separator();

		ImGui::Text("Set first scene");
		if (ImGui::Button("Select start scene"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::Scene;
			GUIProjectItemsList::selectStartScene = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
	}
}
#endif