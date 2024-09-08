//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

#include "gui.hpp"
#include "render.hpp"
#include "configFile.hpp"
#include "core.hpp"
#include "compile.hpp"
#include "fileUtils.hpp"

using std::cout;

using Core::ConfigFile;
using Core::Compiler;
using Core::TheCompiler;
using Utils::File;

namespace Graphics
{
	static ImVec4 bgrColor;

	void GUI::GUIInitialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Compiler::docsPath + "\\imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Compiler::filesPath + "\\fonts\\coda\\Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		io.FontGlobalScale = stof(ConfigFile::GetValue("fontScale"));
	}

	int GUI::GetScreenWidth()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return width;
	}

	int GUI::GetScreenHeight()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return height;
	}

	void GUI::GUILoop()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiDockNodeFlags dockFlags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

		RenderParentWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUI::RenderParentWindow()
	{
		int width = GetScreenWidth();
		int height = GetScreenHeight();
		ImGui::SetNextWindowSize(ImVec2(
			static_cast<float>(width),
			static_cast<float>(height)),
			ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin("##Main", NULL, windowFlags))
		{
			RenderWindowContent();
		}
	}

	void GUI::RenderWindowContent()
	{
		string text = "";
		if (!isBuilding
			&& !hasBuiltOnce)
		{
			text = "Waiting for input...";
		}
		else if (isBuilding)
		{
			text = "Building " + compilableProgramName + "...";
		}
		else if (!isBuilding
				 && hasBuiltOnce)
		{
			text = "Finished building " + compilableProgramName + "...\n";
		}

		if (!isBuilding)
		{
			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 topButtonSize(120, 50);

			ImVec2 topButton1Pos(
				(windowSize.x * 0.4f) - (topButtonSize.x * 0.5f), 15.0f);
			ImVec2 topButton2Pos(
				(windowSize.x * 0.6f) - (topButtonSize.x * 0.5f), 15.0f);
			ImVec2 topButton3Pos(
				(windowSize.x * 0.95f) - (topButtonSize.x * 0.5f), 25.0f);

			ImGui::SetCursorPos(topButton1Pos);
			if (ImGui::Button("Hub", topButtonSize))
			{
				if (compilableProgramName != "Elypso hub")
				{
					compilableProgramName = "Elypso hub";
					string windowTitle = "Compiler | " + compilableProgramName;
					glfwSetWindowTitle(Render::window, windowTitle.c_str());

					cout << "compile " + compilableProgramName + "...\n";
					output.emplace_back("compile " + compilableProgramName + "...");
				}
			}

			ImGui::SetCursorPos(topButton2Pos);
			if (ImGui::Button("Engine", topButtonSize))
			{
				if (compilableProgramName != "Elypso engine")
				{
					compilableProgramName = "Elypso engine";
					string windowTitle = "Compiler | " + compilableProgramName;
					glfwSetWindowTitle(Render::window, windowTitle.c_str());

					cout << "compile " + compilableProgramName + "...\n";
					output.emplace_back("compile " + compilableProgramName + "...");
				}
			}

			ImGui::SetCursorPos(topButton3Pos);
			ImVec2 smallButtonSize = ImVec2(topButtonSize.x * 0.7f, topButtonSize.y * 0.7f);
			if (ImGui::Button("Set path", smallButtonSize))
			{
				targetFolder = File::SetPath();
			}
		}

		ImGui::SetCursorPosY(75.0f);

		ImGui::Text(text.c_str());

		ImVec2 scrollingRegionSize(
			ImGui::GetContentRegionAvail().x,
			ImGui::GetContentRegionAvail().y - 75);
		if (ImGui::BeginChild("ScrollingRegion", scrollingRegionSize, true))
		{
			float wrapWidth = ImGui::GetContentRegionAvail().x - 10;
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

			//display the content of the text buffer
			for (const auto& message : output)
			{
				ImGui::TextWrapped("%s", message.c_str());

				if (ImGui::IsItemClicked()
					&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					cout << "Added '" << message << "' to clipboard.\n";
					ImGui::SetClipboardText(message.c_str());
				}
			}

			ImGui::PopTextWrapPos();

			//scrolls to the bottom if scrolling is allowed
			//and if user is close to the newest compilation message
			bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
			if (isNearBottom
				|| (!firstScrollToBottom))
			{
				ImGui::SetScrollHereY(1.0f);
				firstScrollToBottom = true;
			}
		}

		ImGui::EndChild();

		if (!isBuilding)
		{
			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 buttonSize(120, 50);

			ImVec2 button1Pos(
				(windowSize.x * 0.3f) - (buttonSize.x * 0.5f),
				windowSize.y - buttonSize.y - 15.0f);
			ImVec2 button2Pos(
				(windowSize.x * 0.5f) - (buttonSize.x * 0.5f),
				windowSize.y - buttonSize.y - 15.0f);
			ImVec2 button3Pos(
				(windowSize.x * 0.7f) - (buttonSize.x * 0.5f),
				windowSize.y - buttonSize.y - 15.0f);

			ImGui::SetCursorPos(button1Pos);
			if (ImGui::Button("Clean rebuild", buttonSize))
			{
				cout << "Started clean rebuilding " + compilableProgramName + "\n";
				output.emplace_back("Started clean rebuilding " + compilableProgramName);

				TheCompiler::CleanRebuild();
			}

			ImGui::SetCursorPos(button2Pos);
			if (ImGui::Button("Compile", buttonSize))
			{
				cout << "Started compiling " + compilableProgramName + "\n";
				output.emplace_back("Started compiling " + compilableProgramName);

				TheCompiler::Compile();
			}

			ImGui::SetCursorPos(button3Pos);
			if (ImGui::Button("Clear output", buttonSize))
			{
				output.clear();

				TheCompiler::CleanRebuild();
			}
		}

		ImGui::End();
	}

	void GUI::GUIShutdown()
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
}