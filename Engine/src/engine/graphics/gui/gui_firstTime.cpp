//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_firstTime.hpp"
#include "configFile.hpp"
#include "gui_engine.hpp"

using EngineFile::ConfigFile;

namespace Graphics::GUI
{
	void GUIFirstTime::RenderFirstTime()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(500.0f, 500.0f), ImVec2(5000.0f, 5000.0f));

		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderFirstTime = stoi(ConfigFile::GetValue("gui_firstTime"));

		if (renderFirstTime
			&& ImGui::Begin("Welcome", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_firstTime", "0");
				ConfigFile::SaveConfigFile();
			}

			RenderFirstTimeContent();

			ImGui::End();
		}
	}

	void GUIFirstTime::RenderFirstTimeContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		const char* text1 = "Welcome! If this is your first time using the engine then this window will give a brief introduction of what exists in this engine. So far the engine is in an early alpha state so things may change a lot so please be sure to delete the document folders for engine and hub and the project folder every time you run a new version of the engine.";

		// Padding from edges
		float padding = 25.0f;

		// Calculate wrapping position based on cursor + window size
		float wrapPos = windowSize.x - padding;

		// Set cursor with padding for left side
		int currentHeight = 100.0f;
		ImGui::SetCursorPosY(currentHeight);

		// Apply text wrapping relative to the cursor
		ImGui::PushTextWrapPos(wrapPos);
		ImGui::Text("%s", text1);
		ImGui::PopTextWrapPos();

		const char* text2 = "Welcome! If this is your first time using the engine then this window will give a brief introduction of what exists in this engine. So far the engine is in an early alpha state so things may change a lot so please be sure to delete the document folders for engine and hub and the project folder every time you run a new version of the engine.";
		currentHeight += 150.0f;
		ImGui::SetCursorPosY(currentHeight);

		ImGui::PushTextWrapPos(wrapPos);
		ImGui::Text("%s", text2);
		ImGui::PopTextWrapPos();

		const char* text3 = "Welcome! If this is your first time using the engine then this window will give a brief introduction of what exists in this engine. So far the engine is in an early alpha state so things may change a lot so please be sure to delete the document folders for engine and hub and the project folder every time you run a new version of the engine.";
		currentHeight += 150.0f;
		ImGui::SetCursorPosY(currentHeight);

		ImGui::PushTextWrapPos(wrapPos);
		ImGui::Text("%s", text3);
		ImGui::PopTextWrapPos();

		const char* text4 = "Welcome! If this is your first time using the engine then this window will give a brief introduction of what exists in this engine. So far the engine is in an early alpha state so things may change a lot so please be sure to delete the document folders for engine and hub and the project folder every time you run a new version of the engine.";
		currentHeight += 150.0f;
		ImGui::SetCursorPosY(currentHeight);

		ImGui::PushTextWrapPos(wrapPos);
		ImGui::Text("%s", text4);
		ImGui::PopTextWrapPos();

		const char* text5 = "Welcome! If this is your first time using the engine then this window will give a brief introduction of what exists in this engine. So far the engine is in an early alpha state so things may change a lot so please be sure to delete the document folders for engine and hub and the project folder every time you run a new version of the engine.";
		currentHeight += 150.0f;
		ImGui::SetCursorPosY(currentHeight);

		ImGui::PushTextWrapPos(wrapPos);
		ImGui::Text("%s", text5);
		ImGui::PopTextWrapPos();
	}
}
#endif