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
#include <filesystem>

using std::cout;
using std::wstring;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::ios;
using std::filesystem::is_empty;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::remove_all;
using std::filesystem::rename;
using std::filesystem::copy;
using std::filesystem::create_directory;

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

	Core::projectsFilePath = Core::docsPath.string() + "/projects.txt";
	if (!exists(Core::projectsFilePath))
	{
		ofstream projectsFile(Core::projectsFilePath);
		if (!projectsFile.is_open())
		{
			cout << "Error: Failed to create " << Core::projectsFilePath << "!\n\n";
		}
		projectsFile.close();
	}

	Core::configFilePath = Core::docsPath.string() + "/config.txt";
	if (!exists(Core::configFilePath))
	{
		ofstream configFile(Core::configFilePath);
		if (!configFile.is_open())
		{
			cout << "Error: Failed to create " << Core::configFilePath << "!\n\n";
		}
		configFile.close();
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

	GUI::RenderPanels();
	GUI::RenderButtons();

	if (renderConfirmWindow) GUI::ConfirmRemove(confirmFileName, confirmFilePath);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderPanels()
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

	if (files.size() > 0)
	{
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

			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
				&& !renderConfirmWindow)
			{
				ImGui::SetWindowFocus();
			}

			string fileName = path(file).stem().string();
			ImGui::Text("%s", fileName.c_str());

			if (ImGui::Button("Launch", ImVec2(200, 50)))
			{
				if (!renderConfirmWindow
					&& IsValidEnginePath(Core::enginePath.string()))
				{
					GUI::RunProject(file);
				}
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

			if (ImGui::Button("Remove", ImVec2(200, 50)))
			{
				if (!renderConfirmWindow)
				{
					confirmFileName = fileName;
					confirmFilePath = file;
					renderConfirmWindow = true;
				}
			}

			ImGui::End();

			nextPanelPos.y += panelHeight + panelSpacing;
		}
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
		if (!renderConfirmWindow) GUI::NewProject();
	}

	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	if (ImGui::Button("Add Project", ImVec2(285, 50)))
	{
		if (!renderConfirmWindow) GUI::AddProject();
	}

	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	if (ImGui::Button("Set engine path", ImVec2(285, 50)))
	{
		if (!renderConfirmWindow) GUI::SetEnginePathBySelection();
	}

	ImGui::End();
}

void GUI::NewProject()
{
	string filePath = SelectWithExplorer(SelectType::new_folder);

	path convertedFilePath = path(filePath);
	string fileName = convertedFilePath.stem().string();
	
	char* word = new char[fileName.length() + 1];
	fileName.copy(word, fileName.length());
	word[fileName.length()] = '\0';

	bool foundSpace = false;
	for (int i = 0; i < fileName.length(); ++i)
	{
		if (isspace(word[i]))
		{
			foundSpace = true;
			break;
		}
	}

	delete[] word;

	if (foundSpace)
	{
		cout << "Error: Failed to create new project with name '" << fileName << "' because it contains spaces!\n\n";
		remove_all(filePath);
		return;
	}

	if (filePath.empty())
	{
		cout << "Cancelled folder selection...\n\n";
		return;
	}

	if (!is_empty(filePath))
	{
		cout << "Error: Cannot create a new project inside a folder with content inside it!\n\n";
		remove_all(filePath);
		return;
	}

	path parentPath(filePath);
	parentPath = parentPath.parent_path();
	for (const auto& entry : directory_iterator(parentPath))
	{
		path entryFile(entry);
		path projectFile(filePath + ".project");
		if (entry.is_regular_file()
			&& entryFile.stem() == projectFile.stem())
		{
			cout << "Error: A project with the name '" << projectFile.stem().string() << "' already exists in the same folder!\n\n";
			remove_all(filePath);
			return;
		}
	}

	string scenePath = filePath + "/TemplateScene.txt";
	ofstream scene(scenePath);
	if (!scene.is_open())
	{
		cout << "Error: Failed to open scene file at '" << scenePath << "'!\n\n";
		remove_all(filePath);
		return;
	}

	scene.close();

	string sceneDirectory = filePath + "/TemplateScene";
	create_directory(sceneDirectory);

	//compress scene folder into zip
	rename(scenePath, sceneDirectory + "/TemplateScene.txt");
	string sceneCompressPath = path(sceneDirectory).string() + "\\" + path(sceneDirectory).stem().string() + ".zip";
	if (!Compression::CompressFolder(sceneDirectory, sceneCompressPath))
	{
		cout << "Error: Failed to compress '" << filePath << "'!\n\n";
		remove_all(filePath);
		return;
	}
	//rename scene folder .zip extension to .scene extension
	path sceneFilePath(scenePath);
	string sceneParentProjectPath = sceneFilePath.parent_path().string();
	string sceneScenePath = sceneParentProjectPath + "/" + sceneFilePath.stem().string() + ".scene";
	rename(sceneCompressPath, sceneScenePath);
	
	//move .scene file to parent project folder
	rename(sceneScenePath, filePath + "/" + sceneFilePath.stem().string() + ".scene");
	//remove no longer needed scene folder
	remove_all(sceneDirectory);

	//compress parent folder into zip
	string compressPath = path(filePath).string() + "\\" + path(filePath).stem().string() + ".zip";
	if (!Compression::CompressFolder(filePath, compressPath))
	{
		cout << "Error: Failed to compress '" << filePath << "'!\n\n";
		remove_all(filePath);
		return;
	}
	//rename parent folder .zip extension to .project extension
	path compressFilePath(filePath);
	string parentProjectPath = compressFilePath.parent_path().string();
	string projectPath = parentProjectPath + "/" + compressFilePath.stem().string() + ".project";
	rename(compressPath, projectPath);

	remove_all(filePath);

	ofstream projectsFile(Core::projectsFilePath, ios::app);
	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}
	projectsFile << projectPath << "\n";
	projectsFile.close();
	UpdateFileList();

	cout << "Successfully created new project at '" << projectPath << "'!\n\n";
}

void GUI::AddProject()
{
	string filePath = SelectWithExplorer(SelectType::existing_file);

	if (filePath.empty())
	{
		cout << "Cancelled project selection...\n\n";
		return;
	}
	 
	for (const auto& element : files)
	{
		if (element == filePath)
		{
			cout << "Error: '" << filePath << "' has already been added!\n\n";
			UpdateFileList();
			return;
		}
	}

	ofstream projectsFile(Core::projectsFilePath, ios::app);
	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}
	projectsFile << filePath << "\n";
	projectsFile.close();
	UpdateFileList();

	cout << "Added existing project '" << filePath << "'!\n\n";
}

void GUI::SetEnginePathBySelection()
{
	string filePath = SelectWithExplorer(SelectType::engine_path);

	if (filePath.empty())
	{
		cout << "Cancelled engine selection...\n\n";
		return;
	}

	path enginePath(filePath);
	if (enginePath.stem().string() != "Elypso engine"
		|| enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path " << filePath << " does not lead to Elypso engine.exe!\n\n";
		return;
	}

	ofstream configFile(Core::configFilePath);
	if (!configFile.is_open())
	{
		cout << "Error: Failed to open config file!\n\n";
		return;
	}
	configFile << filePath << "\n";
	configFile.close();

	Core::enginePath = filePath;

	cout << "Set engine path to '" << Core::enginePath << "'!\n\n";
}
void GUI::SetEnginePathFromConfigFile()
{
	ifstream configFile(Core::configFilePath);
	if (!configFile.is_open())
	{
		cout << "Error: Failed to open config file at '" << Core::configFilePath << "'!\n\n";
		return;
	}

	string firstLine;
	if (!getline(configFile, firstLine))
	{
		if (configFile.eof()) cout << "Error: Config file is empty!\n\n";
		else cout << "Error: Couldn't read first line from file!\n\n";
		configFile.close();
		return;
	}

	if (firstLine.empty())
	{
		cout << "Error: Config file is empty!\n\n";
		configFile.close();
		return;
	}

	if (!exists(firstLine))
	{
		cout << "Error: Engine path '" << firstLine << "' read from config file does not exist!\n\n";
		configFile.close();
		return;
	}

	path enginePath(firstLine);
	if (enginePath.stem().string() != "Elypso engine"
		|| enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path '" << firstLine << "' does not lead to Elypso engine.exe!\n\n";
		configFile.close();
		return;
	}
	Core::enginePath = enginePath.string();

	configFile.close();

	cout << "Set engine path to " << Core::enginePath << "!\n\n";
}

void GUI::ConfirmRemove(const string& projectName, const string& projectPath)
{
	ImGui::SetNextWindowPos(ImVec2(400, 200));
	ImGui::SetNextWindowSize(ImVec2(500, 300));

	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoSavedSettings;

	string title = "Remove " + projectName + "?";
	ImGui::Begin(title.c_str(), nullptr, flags);

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 windowCenter(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
	ImVec2 buttonSize(120, 50);
	float buttonSpacing = 20.0f;

	ImVec2 buttonYesPos(
		windowSize.x * 0.45f - buttonSize.x,
		windowSize.y * 0.5f - buttonSize.y * 0.5f);
	ImVec2 buttonNoPos(
		windowSize.x * 0.55f,
		windowSize.y * 0.5f - buttonSize.y * 0.5f);
	
	ImGui::SetCursorPos(buttonYesPos);
	if (ImGui::Button("Yes", buttonSize))
	{
		GUI::RemoveProject(projectPath);
		renderConfirmWindow = false;
	}

	ImGui::SetCursorPos(buttonNoPos);
	if (ImGui::Button("No", buttonSize))
	{
		cout << "Cancelled project removal...\n\n";
		renderConfirmWindow = false;
	}

	ImGui::End();
}
void GUI::RemoveProject(const string& projectPath)
{
	if (!exists(projectPath))
	{
		cout << "Error: Tried to remove '" << projectPath << "' but it has already been removed!\n\n";
		UpdateFileList();
		return;
	}

	std::filesystem::remove(projectPath);
	UpdateFileList();

	cout << "Removed '" << projectPath << "'...\n\n";
}

bool GUI::IsValidEnginePath(const string& enginePath)
{
	if (Core::enginePath == "")
	{
		SetEnginePathFromConfigFile();
	}

	if (Core::enginePath == "")
	{
		cout << "Error: Couldn't run engine because no valid path could be loaded!\n\n";
		return false;
	}

	if (!exists(Core::enginePath))
	{
		cout << "Error: Tried to run '" << Core::enginePath << "' but it doesn't exist!\n\n";
		return false;
	}

	if (Core::enginePath.stem().string() != "Elypso engine"
		|| Core::enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path '" << Core::enginePath << "' does not lead to Elypso engine.exe!\n\n";
		return false;
	}

	return true;
}

void GUI::RunProject(const string& targetProject)
{
	//first we need a duplicate of the original file
	path targetProjectPath(targetProject);
	string copiedTargetProject = 
		targetProjectPath.parent_path().string() + "/TEMP_" + targetProjectPath.stem().string() + ".project";
	copy(targetProjectPath.string(), copiedTargetProject);

	//then we need to rename its extension to zip
	path copiedTargetProjectPath(copiedTargetProject);
	string renamedCopiedTargetProjectPath =
		copiedTargetProjectPath.parent_path().string() + "/" + copiedTargetProjectPath.stem().string() + ".zip";
	rename(copiedTargetProjectPath.string(), renamedCopiedTargetProjectPath);

	//then we can decompress it
	string decompressedTargetProject = 
		copiedTargetProjectPath.parent_path().string() + "/" + copiedTargetProjectPath.stem().string();
	if (!Compression::DecompressFile(renamedCopiedTargetProjectPath, decompressedTargetProject))
	{
		cout << "Error: Failed to decompress '" << targetProjectPath << "'!\n\n";
		//delete temporary zip file
		remove(renamedCopiedTargetProjectPath.c_str());
		return;
	}

	//empty engine folder content if any content exists
	string engineFilesFolderPath = Core::enginePath.parent_path().string() + "/files";
	for (const auto& entry : directory_iterator(engineFilesFolderPath))
	{
		path entryPath = entry.path();
		if (is_regular_file(entryPath)) remove(entryPath);
		else if (is_directory(entryPath)) remove_all(entryPath);
	}

	//move temp folder content to engine files folder content
	for (const auto& entry : directory_iterator(decompressedTargetProject))
	{
		path entryPath = entry.path();
		if (is_regular_file(entryPath))
		{
			string name = entryPath.stem().string();
			string extension = entryPath.extension().string();
			rename(entryPath, engineFilesFolderPath + "/" + name + extension);
		}
		else if (is_directory(entryPath))
		{
			string name = entryPath.stem().string();
			rename(entryPath, engineFilesFolderPath + "/" + name);
		}
	}

	//delete temporary zip file
	remove(renamedCopiedTargetProjectPath.c_str());
	//delete temporary folder
	remove_all(decompressedTargetProject);

	cout << "Running engine from '" << Core::enginePath << "'!\n\n";

	cout << ".\n.\n.\n\n";

	//
	// RUN ENGINE
	//

	wstring exeDir = Core::enginePath.parent_path().wstring();
	wstring wideExePath = Core::enginePath;
	wstring wideCmdArgs = L""; //initialize with your command line arguments

	//initialize structures for process creation
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// Create the new process
	if (!CreateProcessW
	(
		wideExePath.c_str(), //path to the executable
		wideCmdArgs.data(),  //command line arguments
		nullptr,             //process handle not inheritable
		nullptr,             //thread handle not inheritable
		FALSE,               //handle inheritance
		0,                   //creation flags
		nullptr,             //use parent's environment block
		exeDir.c_str(),      //use parent's starting directory
		&si,                 //pointer to STARTUPINFO structure
		&pi                  //pointer to PROCESS_INFORMATION structure
	))
	{
		//retrieve the error code and print a descriptive error message
		LPVOID lpMsgBuf = nullptr;
		DWORD dw = GetLastError();
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER 
			| FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, 
			dw, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPWSTR)&lpMsgBuf, 0, nullptr);
		std::wcout << L"Error: " << reinterpret_cast<LPCWSTR>(lpMsgBuf) << L"\n\n";
		LocalFree(lpMsgBuf);
	}

	// Close process and thread handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void GUI::UpdateFileList()
{
	files.clear();

	ifstream projectsFile(Core::projectsFilePath);

	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}

	string line;
	while (getline(projectsFile, line))
	{
		if (!exists(line)) foundInvalidPath = true;
		else files.push_back(line);
	}

	projectsFile.close();

	if (files.size() > 0
		&& foundInvalidPath)
	{
		ofstream editedProjectsFile(Core::projectsFilePath);
		if (!editedProjectsFile.is_open())
		{
			cout << "Error: Failed to open projects file!\n\n";
			return;
		}

		for (const auto& file : files)
		{
			editedProjectsFile << file << "\n";
		}

		editedProjectsFile.close();

		foundInvalidPath = false;
	}
}

string GUI::SelectWithExplorer(SelectType selectType)
{
	//initialize COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		cout << "Error: Failed to initialize COM!\n\n";
		return "";
	}

	//create an instance of the File Open dialog
	IFileOpenDialog* pFileOpen = nullptr;
	hr = CoCreateInstance(
		CLSID_FileOpenDialog, 
		NULL,
		CLSCTX_ALL,
		IID_IFileOpenDialog,
		reinterpret_cast<void**>(&pFileOpen));
	if (FAILED(hr))
	{
		cout << "Error: Failed to create File Open dialog!\n\n";
		CoUninitialize();
		return "";
	}

	if (selectType == SelectType::new_folder)
	{
		//restrict the selection to folders only
		DWORD dwOptions;
		hr = pFileOpen->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set options!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}
		else
		{
			cout << "Error: Failed to get options!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	else if (selectType == SelectType::existing_file)
	{
		//restrict file selection to .txt only
		COMDLG_FILTERSPEC filterSpec[] = { { L"Project Files", L"*.project"} };
		hr = pFileOpen->SetFileTypes(1, filterSpec);
		if (FAILED(hr))
		{
			cout << "Error: Failed to set file filter!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	else if (selectType == SelectType::engine_path)
	{
		//restrict file selection to .txt only
		COMDLG_FILTERSPEC filterSpec[] = { { L"Executables", L"*.exe"} };
		hr = pFileOpen->SetFileTypes(1, filterSpec);
		if (FAILED(hr))
		{
			cout << "Error: Failed to set file filter!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	else if (selectType == SelectType::scene_file)
	{
		//restrict file selection to .txt only
		COMDLG_FILTERSPEC filterSpec[] = { { L"Scene files", L"*.txt"} };
		hr = pFileOpen->SetFileTypes(1, filterSpec);
		if (FAILED(hr))
		{
			cout << "Error: Failed to set file filter!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	//show the File Open dialog
	hr = pFileOpen->Show(NULL);
	if (FAILED(hr))
	{
		cout << "Error: Failed to show dialog!\n\n";
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//get the result of the user's selection
	IShellItem* pItem;
	hr = pFileOpen->GetResult(&pItem);
	if (FAILED(hr))
	{
		cout << "Error: Failed to retrieve result!\n\n";
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//get the path pf the selected file or folder
	PWSTR filePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (FAILED(hr))
	{
		cout << "Error: Failed to retrieve path!\n\n";
		pItem->Release();
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//convert the wide string to a string
	wstring ws(filePath);
	string selectedPath(ws.begin(), ws.end());

	//free memory allocated for filePath
	CoTaskMemFree(filePath);

	//release the shell item
	pItem->Release();

	//release the file open dialog
	pFileOpen->Release();

	//uninitialze COM
	CoUninitialize();

	return selectedPath;
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