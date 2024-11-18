//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>
#include <fstream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "createproject.hpp"
#include "core.hpp"
#include "gui.hpp"

using Core::Hub;

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::filesystem::path;
using std::ofstream;
using std::filesystem::remove_all;
using std::filesystem::exists;

namespace Graphics::GUI
{
	void CreateProject::RenderCreateProjectWindow()
	{
		ImVec2 size = ImVec2(400.0f, 300.0f);
		ImVec2 windowPos = GUI_Hub::CenterWindow(size);

		ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoTitleBar;

		if (renderCreateProjectWindow
			&& ImGui::Begin("Create project", NULL, windowFlags))
		{
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
			{
				ImGui::SetWindowFocus();
			}

			RenderCreateProjectContent();

			ImGui::End();
		}
	}

	void CreateProject::RenderCreateProjectContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		string text = "Set new project name";
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float offsetX = (windowSize.x - textWidth) * 0.5f;
		if (offsetX > 0.0f) ImGui::SetCursorPos(ImVec2(offsetX, 30));

		ImGui::Text("%s", text.c_str());

		ImVec2 textfieldPos = ImVec2(
			windowSize.x / 2 - 125,
			windowSize.y / 2 - 50);

		ImGui::SetCursorPos(textfieldPos);

#ifdef _WIN32
		strcpy_s(projectName, bufferSize, assignedProjectName.c_str());
#elif __linux__
		strncpy(projectName, assignedProjectName.c_str(), bufferSize);
#endif
		if (ImGui::InputText("##setProjectName", projectName, bufferSize))
		{
			assignedProjectName = projectName;
		}

		if (strlen(projectName) == 0)
		{
#ifdef _WIN32
			strcpy_s(projectName, bufferSize, "Project");
#elif __linux__
			strncpy(projectName, "Project", bufferSize);
#endif
		}
		if (assignedProjectName == "") assignedProjectName = "Project";

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Create", buttonSize))
		{
			//
			// CHECK IF NAME HAS ILLEGAL CHARACTER
			//

			bool foundIllegalChar = false;
			char illegalChar;
			for (char c : assignedProjectName)
			{
				if (!IsValidSymbolInPath(c))
				{
					foundIllegalChar = true;
					illegalChar = c;
					break;
				}
			}
			if (foundIllegalChar)
			{
				string str(1, illegalChar);
				cout << "Error: Invalid character '" + str + "' detected in project name '" + assignedProjectName + "'! Please only use english letters, roman numbers and dash or underscore symbol!";

#ifdef _WIN32
				strcpy_s(projectName, bufferSize, "Project");
#elif __linux__
				strncpy(projectName, "Project", bufferSize);
#endif
				assignedProjectName = "Project";

				renderCreateProjectWindow = false;

				return;
			}

			//
			// CHECK IF PROJECT FOLDER WITH SAME NAME ALREADY EXISTS
			//

			bool foundExistingProject = false;

			for (const auto& entry : directory_iterator(Hub::projectsFolderPath))
			{
				if (entry.path().filename().string() == assignedProjectName)
				{
					foundExistingProject = true;
					break;
				}
			}

			if (foundExistingProject)
			{
#ifdef _WIN32
				strcpy_s(projectName, bufferSize, "Project");
#elif __linux__
				strncpy(projectName, "Project", bufferSize);
#endif
				assignedProjectName = "Project";

				cout << "Error: Project name '" + assignedProjectName + "' already exists in this project! Please pick a new project name.";

				renderCreateProjectWindow = false;

				return;
			}

			//
			// CREATE NEW PROJECT
			//

			//create parent project folder
			string newProjectFile = (path(Hub::projectsFolderPath.string()) / assignedProjectName).string();
			create_directory(newProjectFile);

			//create scenes folder
			string scenesFolder = (path(newProjectFile) / "scenes").string();
			create_directory(scenesFolder);

			//create textures folder inside parent project folder
			string texturesFolder = (path(newProjectFile) / "textures").string();
			create_directory(texturesFolder);

			//put project.txt inside engine documents folder
			string engineDocsFolder = (path(Hub::docsPath).parent_path() / "Elypso engine").string();
			if (!exists(engineDocsFolder)) create_directory(engineDocsFolder);
			string projectFilePath = (path(engineDocsFolder) / "project.txt").string();
			ofstream projectFile(projectFilePath);
			if (!projectFile.is_open())
			{
				cout << "Error: Failed to open project file at '" << projectFilePath << "'!\n\n";
				remove_all(newProjectFile);
				renderCreateProjectWindow = false;
			}

			string engineParentPath = Hub::enginePath.parent_path().string();
			projectFile << "project: " << newProjectFile + "\n";
			projectFile.close();

			GUI_Hub::UpdateFileList();

			cout << "Created new project '" << newProjectFile << "'\n";

			renderCreateProjectWindow = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			renderCreateProjectWindow = false;
		}
	}
}