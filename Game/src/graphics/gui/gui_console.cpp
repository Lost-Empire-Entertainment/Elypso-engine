//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//external
#include "imgui.h"
#include "imgui_internal.h"
#include "glfw3.h"

//game
#include "gui_console.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "render.hpp"
#include "console.hpp"

using std::cout;

using Core::Game;
using Graphics::Render;
using Core::ConsoleManager;

namespace Graphics::GUI
{
	void GUIConsole::RenderConsole()
	{
		float windowWidth = static_cast<float>(GameGUI::GetScreenWidth());
		float windowHeight = static_cast<float>(GameGUI::GetScreenHeight());

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight / 3), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings;

		if (renderConsole
			&& ImGui::Begin("Console", NULL, windowFlags))
		{
			RenderConsoleContent();
		}

		ImGui::End();
	}

	void GUIConsole::RenderConsoleContent()
	{
		//text area with scrollable region
		ImVec2 scrollingRegionSize(
			ImGui::GetContentRegionAvail().x,
			ImGui::GetContentRegionAvail().y - 40);
		if (ImGui::BeginChild("ScrollingRegion", scrollingRegionSize, true))
		{
			float wrapWidth = ImGui::GetContentRegionAvail().x - 10;
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

			//display the content of the text buffer
			for (const auto& message : consoleMessages)
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
			//and if user is close to the newest console message
			bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
			if (isNearBottom
				|| (!firstScrollToBottom
					&& Game::startedWindowLoop))
			{
				ImGui::SetScrollHereY(1.0f);
				firstScrollToBottom = true;
			}
		}

		ImGui::EndChild();

		//text filter input box
		float textAreaHeight = ImGui::GetContentRegionAvail().y - 25.0f;
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + textAreaHeight - 10.0f));

		//draw the text filter input box
		ImGui::PushItemWidth(scrollingRegionSize.x);
		ImGuiInputTextFlags inputFieldTextFlags =
			ImGuiInputTextFlags_EnterReturnsTrue;

		//press enter to insert install path
		if (ImGui::InputTextWithHint(
			"##inputfield",
			"Enter command...",
			inputTextBuffer,
			sizeof(inputTextBuffer),
			inputFieldTextFlags,
			[](ImGuiInputTextCallbackData* data) -> int
			{
				//allow inserting text with Ctrl+V
				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)) &&
					ImGui::GetIO().KeyCtrl)
				{
					const char* clipboardText = glfwGetClipboardString(Render::window);
					if (clipboardText)
					{
						//insert clipboard text into the input buffer
						int textLength = static_cast<int>(strlen(clipboardText));
						int availableSpace = data->BufSize - data->BufTextLen;
						int copyLength = ImMin(textLength, availableSpace - 1); //leave space for null terminator
						if (copyLength > 0)
						{
							memcpy(data->Buf + data->CursorPos, clipboardText, copyLength);
							data->CursorPos += copyLength;
							data->BufTextLen += copyLength;
							data->Buf[data->CursorPos] = '\0';
						}
					}
				}
				return 0;
			}))
		{
			ConsoleManager::ParseConsoleCommand(inputTextBuffer);
			ImGui::SetScrollHereY(1.0f);
			memset(inputTextBuffer, 0, sizeof(inputTextBuffer));
		}

		ImGui::PopItemWidth();
	}

	void GUIConsole::AddTextToConsole(const string& message)
	{
		consoleMessages.push_back(message);

		//erases the first added message if a new one was added and count exceeded max count
		while (consoleMessages.size() > maxConsoleMessages)
		{
			consoleMessages.erase(consoleMessages.begin());
		}
	}
}