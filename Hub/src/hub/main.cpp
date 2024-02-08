//external
#include "glad.h"
#include "stb_image.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "main.hpp"

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>

using std::cout;
using std::exception;
using std::wstring;
using std::filesystem::current_path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;

int main()
{
	Core::Initialize();

	cout << "Successfully reached render loop!\n";

	while (!glfwWindowShouldClose(Core::window))
	{
		Core::Render();
	}

	Core::Shutdown();

	return 0;
}

//
// CORE
//

void Core::Initialize()
{
	cout << "Initializing GLFW...\n";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	cout << "Initializing window...\n";

	window = glfwCreateWindow(
		SCR_WIDTH,
		SCR_HEIGHT,
		"Elypso hub 1.0.0",
		NULL,
		NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
	glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
	glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
	glfwSwapInterval(1);

	defaultPath = current_path().generic_string();

	int width, height, channels;
	string iconpath = defaultPath.string() + "/icon.png";
	unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

	GLFWimage icon{};
	icon.width = width;
	icon.height = height;
	icon.pixels = iconData;

	glfwSetWindowIcon(window, 1, &icon);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	cout << "Initializing GLAD...\n";

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);

	GUI::Initialize();
}

void Core::UpdateAfterRescale(GLFWwindow* window, int width, int height)
{
	//Calculate the new aspect ratio
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	//Set the viewport based on the aspect ratio
	glViewport(0, 0, width, height);
}

void Core::Render()
{
	glClearColor(
		backgroundColor.x,
		backgroundColor.y,
		backgroundColor.z,
		1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GUI::Render();

	//swap the front and back buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Core::Shutdown()
{
	GUI::Shutdown();

	glfwTerminate();
}

//
// GUI
//

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

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderPanels(const vector<string>& files)
{
	glfwGetFramebufferSize(Core::window, &framebufferWidth, &framebufferHeight);

	ImGui::SetNextWindowSize(ImVec2(900, framebufferHeight));

	ImGuiWindowFlags mainWindowFlags =
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Main", NULL, mainWindowFlags);

	float mainWindowX = framebufferWidth - ImGui::GetWindowSize().x;
	ImGui::SetWindowPos(ImVec2(mainWindowX, 0));

	int height = (minSize.y + panelSpacing) * files.size();
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
		ImGui::SetNextWindowSize(ImVec2(minSize));

		ImGui::Begin(file.c_str(), NULL, windowFlags);

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		{
			ImGui::SetWindowFocus();
		}

		string fileName = path(file).filename().string();
		ImGui::Text("File: %s", fileName.c_str());

		if (ImGui::Button("Launch", ImVec2(200, 50)))
		{
			cout << "launching " << fileName << "\n\n";
		}

		ImGui::End();

		nextPanelPos.y += minSize.y + panelSpacing;
	}

	ImGui::EndChild();

	ImGui::End();
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