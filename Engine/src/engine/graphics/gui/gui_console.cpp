//Copyright(C) 2024 Lost Empire Entertainment
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
#include "gui_engine.hpp"
#include "core.hpp"
#include "console.hpp"
#include "gui_console.hpp"
#include "render.hpp"
#include "configFile.hpp"

using Graphics::Render;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using EngineFile::ConfigFile;

namespace Graphics::GUI
{
	void GUIConsole::RenderConsole()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderConsole = stoi(ConfigFile::GetValue("gui_console"));

		if (renderConsole
			&& ImGui::Begin("Console", NULL, windowFlags))
		{
			if (ImGui::Button("Clear"))
			{
				consoleMessages.clear();
			}

			ImGui::SameLine();
			string printMessage = ConsoleManager::sendDebugMessages == true
				? "Disable debug messages"
				: "Enable debug messages";
			if (ImGui::Button(printMessage.c_str()))
			{
				ConsoleManager::sendDebugMessages = !ConsoleManager::sendDebugMessages;
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_console", "0");
			}

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
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::DEBUG,
							"Added '" + message + "' to clipboard.\n");
						ImGui::SetClipboardText(message.c_str());
					}
				}

				ImGui::PopTextWrapPos();

				//scrolls to the bottom if scrolling is allowed
				//and if user is close to the newest console message
				bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
				if (isNearBottom
					|| (!firstScrollToBottom
						&& Engine::startedWindowLoop))
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

			ImGui::End();
		}
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
#endif