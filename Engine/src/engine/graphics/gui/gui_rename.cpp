//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <iostream>
#include <fstream>
#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_rename.hpp"
#include "gui_engine.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "fileUtils.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "console.hpp"

using std::cout;
using std::ifstream;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::getline;
using std::stringstream;
using std::filesystem::path;

using EngineFile::SceneFile;
using Graphics::Render;
using Utils::File;
using Core::Engine;
using Utils::String;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	void GUIRename::RenderRenameWindow()
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

		if (renderRenameWindow
			&& ImGui::Begin("Rename", NULL, windowFlags))
		{
			RenderRenameContent();

			ImGui::End();
		}
	}

	void GUIRename::RenderRenameContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 textfieldPos = ImVec2(
			windowSize.x / 2 - 125,
			windowSize.y / 2 - 50);

		ImGui::SetCursorPos(textfieldPos);
#ifdef _WIN32
		strcpy_s(name, bufferSize, newName.c_str());
#elif __linux__
		strncpy(name, newName.c_str(), bufferSize);
#endif
		if (ImGui::InputText("##setSceneName", name, bufferSize))
		{
			newName = name;
		}

		if (strlen(name) == 0)
		{
#ifdef _WIN32
			strcpy_s(name, bufferSize, "_");
#elif __linux__
			strncpy(name, "_", bufferSize);
#endif
		}
		if (newName == "") newName = "_";

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Rename", buttonSize))
		{
			//
			// CHECK IF NAME HAS ILLEGAL CHARACTER
			//

			bool foundIllegalChar = false;
			for (char c : newName)
			{
				if (!String::IsValidSymbolInPath(c))
				{
					foundIllegalChar = true;
					break;
				}
			}
			if (foundIllegalChar)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Invalid character detected in file/folder name '" + newName + "'! Please only use english letters, roman numbers and dash, dot or underscore symbol!");

#ifdef _WIN32
				strcpy_s(name, bufferSize, "Name");
#elif __linux__
				strncpy(name, "Name", bufferSize);
#endif
				newName = "Name";

				extension = "";
				originalName = "";
				parentFolder = "";

				renderRenameWindow = false;

				return;
			}

			//
			// CHECK IF FILE WITH SAME NAME ALREADY EXISTS
			//

			bool foundExistingFile = false;

			for (const auto& entry : directory_iterator(parentFolder))
			{
				if (entry.path().filename().string() == newName)
				{
					foundExistingFile = true;
					break;
				}
			}

			if (foundExistingFile)
			{
#ifdef _WIN32
				strcpy_s(name, bufferSize, newName.c_str());
#elif __linux__
				strncpy(name, newName.c_str(), bufferSize);
#endif
				newName = newName;

				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: File/folder name '" + newName + "' already exists in this folder! Please pick a new file/folder name.");

				extension = "";
				originalName = "";
				parentFolder = "";

				renderRenameWindow = false;

				return;
			}

			//
			// FIND OLD MODEL PATH FROM SCENE FILE AND WRITE WITH NEW PATH INTO BUFFER
			//

			string originalNameAndExtension = originalName + extension;
			string originalPath = (path(parentFolder) / originalNameAndExtension).string();
			string newNameAndExtension = newName + extension;
			string newPath = (path(parentFolder) / newNameAndExtension).string();

			stringstream buffer;

			ifstream readSceneFile(Engine::scenePath);
			if (!readSceneFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Failed to open scene file '" + Engine::scenePath + "'!\n\n");

				extension = "";
				originalName = "";
				parentFolder = "";

				renderRenameWindow = false;

				return;
			}

			string readFileLine;
			while (getline(readSceneFile, readFileLine))
			{
				if (readFileLine.find(originalPath) != string::npos)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::DEBUG,
						"Found and replaced old model path '" + originalPath + "' with new path '" + newPath + "'.\n");
					buffer << "model path= " << newPath << "\n";
				}
				else buffer << readFileLine << "\n";
			}
			readSceneFile.close();

			//
			// WRITE NEW MODEL PATH INTO SCENE FILE
			//

			string bufferContent = buffer.str();
			ofstream writeSceneFile(Engine::scenePath);
			if (!writeSceneFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Failed to open scene file '" + Engine::scenePath + "'!\n\n");

				extension = "";
				originalName = "";
				parentFolder = "";

				renderRenameWindow = false;

				return;
			}

			writeSceneFile << bufferContent;

			writeSceneFile.close();

			File::MoveOrRenameFileOrFolder(originalPath, newPath, true);

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

			extension = "";
			originalName = "";
			parentFolder = "";

			renderRenameWindow = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			extension = "";
			originalName = "";
			parentFolder = "";

			renderRenameWindow = false;
		}
	}
}
#endif