//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_links.hpp"
#include "gui.hpp"
#include "browserUtils.hpp"

using Utils::Browser;

namespace Graphics::GUI
{
	void GUILinks::RenderLinksWindow()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderLinksWindow
			&& ImGui::Begin("Links", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderLinksWindow = false;
			}

			RenderLinksContent();

			ImGui::End();
		}
	}

	void GUILinks::RenderLinksContent()
	{
		if (ImGui::BeginChild("##links"))
		{
			ImGui::Separator();
			ImGui::Text("Support development on Paypal");
			ImGui::Separator();

			if (ImGui::Button("Paypal"))
			{
				Browser::OpenLink("https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6");
			}

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Check out the official website for documentation (in development)");
			ImGui::Separator();

			if (ImGui::Button("Website"))
			{
				Browser::OpenLink("https://elypsoengine.com");
			}

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Join the official Discord server");
			ImGui::Separator();

			if (ImGui::Button("Discord"))
			{
				Browser::OpenLink("https://discord.gg/FqJgy2SvDs");
			}

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Subscribe to the Lost Empire Entertainment youtube channel");
			ImGui::Separator();

			if (ImGui::Button("Youtube"))
			{
				Browser::OpenLink("https://youtube.com/greenlaser");
			}

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Check out the repository where all the engine code is stored");
			ImGui::Separator();

			if (ImGui::Button("Repository"))
			{
				Browser::OpenLink("https://github.com/Lost-Empire-Entertainment/Elypso-engine");
			}

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Check out the Trello board to check what has already been done and what is coming in future updates");
			ImGui::Separator();

			if (ImGui::Button("Trello"))
			{
				Browser::OpenLink("https://trello.com/b/hbt6ebCZ/elypso-engine");
			}
		}
		ImGui::EndChild();
	}
}