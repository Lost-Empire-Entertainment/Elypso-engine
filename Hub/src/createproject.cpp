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
#include "main.hpp"
#include "gui.hpp"

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::ofstream;
using std::filesystem::remove_all;
using std::filesystem::exists;

void CreateProject::RenderCreateProjectWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

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

	strcpy_s(projectName, bufferSize, assignedProjectName.c_str());
	if (ImGui::InputText("##setProjectName", projectName, bufferSize))
	{
		assignedProjectName = projectName;
	}

	if (strlen(projectName) == 0) strcpy_s(projectName, bufferSize, "Project");
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

			strcpy_s(projectName, bufferSize, "Project");
			assignedProjectName = "Project";

			renderCreateProjectWindow = false;

			return;
		}

		//
		// CHECK IF PROJECT FOLDER WITH SAME NAME ALREADY EXISTS
		//

		bool foundExistingProject = false;

		for (const auto& entry : directory_iterator(Core::projectsFolderPath))
		{
			if (entry.path().filename().string() == assignedProjectName)
			{
				foundExistingProject = true;
				break;
			}
		}

		if (foundExistingProject)
		{
			strcpy_s(projectName, bufferSize, "Project");
			assignedProjectName = "Project";

			cout << "Error: Project name '" + assignedProjectName + "' already exists in this project! Please pick a new project name.";

			renderCreateProjectWindow = false;

			return;
		}

		//
		// CREATE NEW PROJECT
		//

		//create parent project folder
		string newProjectFile = Core::projectsFolderPath.string() + "\\" + assignedProjectName;
		create_directory(newProjectFile);

		//create scenes folder
		string scenesFolder = newProjectFile + "\\scenes";
		create_directory(scenesFolder);

		//create folder for this scene
		string scene1Folder = scenesFolder + "\\Scene1";
		create_directory(scene1Folder);

		//create gameobjects folder
		string gameobjectsFolder = scene1Folder + "\\gameobjects";
		create_directory(gameobjectsFolder);

		//create textures folder inside parent project folder
		string texturesFolder = newProjectFile + "\\textures";
		create_directory(texturesFolder);

		string sceneFilePath = scene1Folder + "\\scene.txt";
		ofstream sceneFile(sceneFilePath);
		if (!sceneFile.is_open())
		{
			cout << "Error: Failed to open scene file at '" << sceneFilePath << "'!\n\n";
			remove_all(newProjectFile);
			renderCreateProjectWindow = false;
		}
		sceneFile.close();

		string projectFilePath = newProjectFile + "\\project.txt";
		ofstream projectFile(projectFilePath);
		if (!projectFile.is_open())
		{
			cout << "Error: Failed to open project file at '" << projectFilePath << "'!\n\n";
			remove_all(newProjectFile);
			renderCreateProjectWindow = false;
		}

		string engineParentPath = Core::enginePath.parent_path().string();
		projectFile << "scene: " << sceneFilePath + "\n";
		projectFile << "project: " << projectFilePath + "\n";
		projectFile.close();

		GUI::UpdateFileList();

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