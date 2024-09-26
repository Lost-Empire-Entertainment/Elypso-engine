//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#else
#include <string>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_game.hpp"
#include "core.hpp"
#include "fileUtils.hpp"
#include "configFile.hpp"
#include "render.hpp"

using std::string;
using std::filesystem::exists;

using Core::Engine;
using Utils::File;
using EngineFile::ConfigFile;
using Graphics::Render;

namespace Graphics::GUI
{
	void GameGUI::Initialize()
	{
		//copies template file to documents folder if imgui file does not exist
		string imguiConfigFile = Engine::docsPath + "\\imgui.ini";
		string imguiTemplateFile = Engine::filesPath + "\\imgui.ini";
		if (!exists(imguiConfigFile))
		{
			File::CopyFileOrFolder(imguiTemplateFile, imguiConfigFile);

			ConfigFile::SetValue("gui_sceneHierarchy", "1");
			ConfigFile::SetValue("gui_projectHierarchy", "1");
			ConfigFile::SetValue("gui_console", "1");
			ConfigFile::SetValue("gui_inspector", "1");
			ConfigFile::SaveConfigFile();
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Engine::docsPath + "\\imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::filesPath + "\\fonts\\coda\\Coda-Regular.ttf").c_str(), 16.0f);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		io.FontGlobalScale = stof(ConfigFile::GetValue("gui_fontScale"));

		isImguiInitialized = true;
	}

	void GameGUI::Render()
	{
		if (isImguiInitialized)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			//
			//RENDER TOP BAR HERE
			//

			ImGuiDockNodeFlags dockFlags =
				ImGuiDockNodeFlags_PassthruCentralNode;

			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

			if (imguiWindows.size() > 0)
			{
				for (const auto& window : imguiWindows)
				{
					window();
				}
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	ImVec2 GameGUI::CenterWindow(const ImVec2& size)
	{
		int intWidth, intHeight;
		glfwGetFramebufferSize(Render::window, &intWidth, &intHeight);

		float glfwWindowWidth = static_cast<float>(intWidth);
		float glfwWindowHeight = static_cast<float>(intHeight);

		float posX = (glfwWindowWidth - size.x) / 2.0f;
		float posY = (glfwWindowHeight - size.y) / 2.0f;

		return ImVec2(posX, posY);
	}

	void GameGUI::Shutdown()
	{
		isImguiInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
#endif