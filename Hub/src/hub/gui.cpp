//<Elypso hub>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine 

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "gui.hpp"
#include "main.hpp"
#include "compression.hpp"

#include <iostream>
#include <ShlObj.h>
#include <fstream>

using std::cout;
using std::wstring;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::filesystem::exists;
using std::filesystem::directory_iterator;

void GUI::Initialize()
{
	cout << "Initializing ImGui...\n\n";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	PWSTR path;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
	if (SUCCEEDED(result))
	{
		wstring wPath(path);
		CoTaskMemFree(path); //free the allocated memory

		string result = string(wPath.begin(), wPath.end());
		size_t pos = 0;
		string incorrectSlash = "\\";
		string correctSlash = "/";
		while ((pos = result.find(incorrectSlash, pos)) != string::npos)
		{
			result.replace(pos, incorrectSlash.length(), correctSlash);
			pos += correctSlash.length();
		}
		Core::docsPath = result + "/Elypso hub";
	}

	if (!exists(Core::docsPath))
	{
		create_directory(Core::docsPath);
	}

	Core::projectsPath = Core::docsPath.string() + "/projects";
	if (!exists(Core::projectsPath))
	{
		create_directory(Core::projectsPath);
	}

	static string tempString = Core::docsPath.string() + "/imgui.ini";
	const char* customConfigPath = tempString.c_str();
	io.IniFilename = customConfigPath;

	ImGui_ImplGlfw_InitForOpenGL(Core::window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	string fontPath = Core::defaultPath.string() + "/files/fonts/coda/Coda-Regular.ttf";
	if (exists(fontPath))
	{
		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Core::defaultPath.string() + "/files/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);
	}
	else
	{
		cout << "Error: Font " << fontPath << " does not exist!\n\n";
	}

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.TabRounding = 6;
	style.FramePadding = ImVec2(6, 2);
	style.ItemSpacing = ImVec2(0, 5);
	io.FontGlobalScale = 1.5f;
}

void GUI::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	ImGuiDockNodeFlags dockFlags =
		ImGuiDockNodeFlags_PassthruCentralNode;

	GUI::RenderPanels(GetFiles(Core::projectsPath.string()));
	GUI::RenderButtons();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderPanels(const vector<string>& files)
{
	glfwGetFramebufferSize(Core::window, &framebufferWidth, &framebufferHeight);

	ImGuiWindowFlags mainWindowFlags =
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Panels", NULL, mainWindowFlags);

	ImGui::SetWindowPos(ImVec2(300, 0));
	ImGui::SetWindowSize(ImVec2(static_cast<float>(framebufferWidth) - 300, static_cast<float>(framebufferHeight)));

	int height = (panelHeight + panelSpacing) * files.size();
	if (height < framebufferHeight - 20) height = framebufferHeight - 20;
	ImGui::BeginChild("ScrollingRegion", ImVec2(framebufferWidth, height), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImVec2 nextPanelPos = ImGui::GetCursorScreenPos();

	for (const auto& file : files)
	{
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos(nextPanelPos);

		ImGui::Begin(file.c_str(), NULL, windowFlags);

		ImGui::SetWindowSize(ImVec2(framebufferWidth - 335, 200));

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		{
			ImGui::SetWindowFocus();
		}

		string fileName = path(file).stem().string();
		ImGui::Text("%s", fileName.c_str());

		if (ImGui::Button("Launch", ImVec2(200, 50)))
		{
			cout << "launching " << fileName << "\n\n";
		}

		ImGui::End();

		nextPanelPos.y += panelHeight + panelSpacing;
	}

	ImGui::EndChild();

	ImGui::End();
}

void GUI::RenderButtons()
{
	glfwGetFramebufferSize(Core::window, &framebufferWidth, &framebufferHeight);

	ImGui::SetNextWindowSize(ImVec2(300, framebufferHeight));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags mainWindowFlags =
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Buttons", NULL, mainWindowFlags);

	if (ImGui::Button("New Project", ImVec2(285, 50)))
	{
		GUI::NewProject();
	}

	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	if (ImGui::Button("Add Project", ImVec2(285, 50)))
	{
		GUI::AddProject();
	}

	ImGui::End();
}

void GUI::NewProject()
{
	string filePath = SetNewProjectFolderPath(NULL);

	if (filePath.empty())
	{
		cout << "Cancelled folder selection...\n\n";
		return;
	}

	for (const auto& entry : directory_iterator(filePath))
	{
		if (entry.is_regular_file()
			|| entry.is_directory())
		{
			cout << "Error: Cannot create a project inside a folder with content inside it!\n\n";
			return;
		}
	}

	ofstream scene(filePath + "/scene.txt");
	if (!scene.is_open())
	{
		cout << "Error: Failed to open scene file at " << filePath + "/scene.txt\n\n";
		return;
	}

	scene.close();

	string compressPath = path(filePath).parent_path().string() + "\\" + path(filePath).stem().string() + ".project";
	cout << "Attempting to compress " << filePath << " to " << compressPath << "\n\n";
	if (!Compression::Compress(filePath, compressPath))
	{
		cout << "Error: Failed to compress the folder " << filePath << "!\n\n";
		return;
	}

	cout << "Successfully created new project at " << filePath << "!\n\n";
}

void GUI::AddProject()
{
	cout << "Added existing project" << "\n\n";
}

vector<string> GUI::GetFiles(const string& path)
{
	vector<string> files;
	files.clear();

	//add files that exist in the projects directory
	for (const auto& entry : directory_iterator(path))
	{
		if (entry.path().extension() == ".project")
		{
			const string filePath = entry.path().string();

			if (find(files.begin(), files.end(), filePath) == files.end())
			{
				files.push_back(filePath);
			}
		}
	}

	return files;
}

string GUI::SetNewProjectFolderPath(HWND hwndOwner)
{
	TCHAR szDisplayName[MAX_PATH]{};
	BROWSEINFO browseInfo;
	ZeroMemory(&browseInfo, sizeof(browseInfo));
	browseInfo.hwndOwner = hwndOwner;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = TEXT("Select a folder");
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);
	if (pidl != NULL)
	{
		//get the path from the PIDL
		if (SHGetPathFromIDList(pidl, szDisplayName))
		{
			//free the PIDL
			IMalloc* pMalloc;
			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				pMalloc->Free(pidl);
				pMalloc->Release();
			}
			return string(szDisplayName);
		}
	}
	return string();
}

void GUI::Shutdown()
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