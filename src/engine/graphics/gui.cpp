//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "timeManager.hpp"
#include "searchUtils.hpp"
#include "fileUtils.hpp"
#include "browserUtils.hpp"
#include "shutdown.hpp"

#include <string>
#include <filesystem>

using std::cout;
using std::endl;
using std::stoi;
using std::to_string;
using std::exception;
using std::filesystem::exists;
using glm::clamp;

using Core::Engine;
using Core::Input;
using Core::ConsoleManager;
using Core::TimeManager;
using Core::ShutdownManager;
using Utils::Search;
using Utils::File;
using Utils::Browser;
using Utils::String;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

	GUI& GUI::GetInstance()
	{
		static GUI instance;
		return instance;
	}

	void GUI::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Engine::docsPath + "/imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Engine::filesPath + "/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		cubeColor.x = Render::cubeColor.x;
		cubeColor.y = Render::cubeColor.y;
		cubeColor.z = Render::cubeColor.z;
		cubeColor.w = 1.0f;

		specularColor.x = Render::specularColor.x;
		specularColor.y = Render::specularColor.y;
		specularColor.z = Render::specularColor.z;
		specularColor.w = 1.0f;

		CustomizeImGuiStyle();
	}

	void GUI::CustomizeImGuiStyle()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = fontScale;
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

	float GUI::GetScreenRefreshRate()
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		return static_cast<float>(videoMode->refreshRate);
	}

	void GUI::Render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		RenderTopBar();

		RenderKeybindsMenu();
		RenderDebugMenu();
		RenderConsole();
		RenderSceneMenu();

		RenderVersionCheckWindow();

		if (Input::inputSettings.printIMGUIToConsole)
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI, 
					Type::DEBUG, 
					"Left mouse button clicked!\n");
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUI::RenderTopBar()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Open is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Save"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Save is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Save As"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Save As is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("New Project"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"New Project is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Exit"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"User closed engine exit button.\n");
				ShutdownManager::shouldShutDown = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(50 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Undo is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Redo"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Redo is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Cut"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Cut is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Copy"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Copy is a placeholder button and does not yet have any functions.\n");
			}

			if (ImGui::MenuItem("Paste"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::DEBUG,
					"Paste is a placeholder button and does not yet have any functions.");
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(100 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Keybinds"))
			{
				showKeybindsMenu = true;
			}

			if (ImGui::MenuItem("Debug menu"))
			{
				showDebugMenu = true;
			}

			if (ImGui::MenuItem("Console"))
			{
				showConsole = true;
			}

			if (ImGui::MenuItem("Scene menu"))
			{
				showSceneMenu = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(180 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Check for Updates"))
			{
				TB_CheckVersion();
			}

			if (ImGui::MenuItem("Report an Issue"))
			{
				TB_ReportIssue();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void GUI::TB_CheckVersion()
	{
		string batFilePath = Engine::filesPath + "/bat scripts/checkVersion.bat";

		if (exists(batFilePath))
		{
			string batOutput = File::GetOutputFromBatFile((batFilePath).c_str());

			vector<string>splitOutput = String::Split(batOutput, ' ');
			vector<string>cleanedOutput = String::RemoveExcept(splitOutput, "0.0.");
			vector<string>sortedOutput = String::RemoveDuplicates(cleanedOutput);

			string currentVersion = String::Replace(Engine::version, " Prototype", "");

			int versionNumber = stoi(String::Split(currentVersion, '.')[2]);
			int currentVersionNumber = 0;
			for (const auto& part : sortedOutput)
			{
				int partVersionNumber = stoi(String::Split(part, '.')[2]);
				if (partVersionNumber > currentVersionNumber)
				{
					currentVersionNumber = partVersionNumber;
				}
			}

			if (currentVersionNumber > versionNumber) outdatedVersion = true;

			versionCompare = 
				"Latest version: 0.0." + to_string(currentVersionNumber) + "\n" +
				" Your version: " + currentVersion;

			versionConfirm += outdatedVersion ?
				"\n\nYour version is out of date!" :
				"\n\nYour version is up to date!";

			showVersionWindow = true;
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Path " + batFilePath + " does not exist!");
		}
	}

	void GUI::TB_ReportIssue()
	{
		try
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::DEBUG,
				"User opened link to Github repository issues page.\n");
			Browser::OpenLink("https://github.com/Lost-Empire-Entertainment/Elypso-engine/issues");
		}
		catch (const exception& e)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Failed to open link to Github repository issues page! " + string(e.what()) + "\n");
		}
	}

	void GUI::RenderKeybindsMenu()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags = 
			ImGuiWindowFlags_NoCollapse;

		if (showKeybindsMenu
			&& ImGui::Begin("Keybinds", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showKeybindsMenu = false;
			}

			ImGui::Text("General keys");
			ImGui::Text("");
			ImGui::Text("Forwards: W");
			ImGui::Text("Backwards: S");
			ImGui::Text("Left: A");
			ImGui::Text("Right: D");
			ImGui::Text("Up: Space");
			ImGui::Text("Down: Left Control");
			ImGui::Text("Sprint: Left Shift");
			ImGui::Text("Toggle camera: Escape");

			ImGui::End();
		}
	}

	void GUI::RenderDebugMenu()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (showDebugMenu
			&& ImGui::Begin("Debug menu", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showDebugMenu = false;
			}

			if (ImGui::BeginTabBar("Debug"))
			{
				if (ImGui::BeginTabItem("Debug info"))
				{
					RWPart_DebugMenuInfo();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Debug interactions"))
				{
					RWPart_Interactions();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

	void GUI::RenderConsole()
	{
		ImVec2 initialPos(357, 500);
		ImVec2 initialSize(530, 200);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (showConsole
			&& ImGui::Begin("Console", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showConsole = false;
			}

			//text area with scrollable region
			ImVec2 scrollingRegionSize(
				ImGui::GetContentRegionAvail().x,
				ImGui::GetContentRegionAvail().y - 25);
			ImGui::BeginChild("ScrollingRegion", scrollingRegionSize, true);

			float wrapWidth = ImGui::GetContentRegionAvail().x - 10;
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

			//display the content of the text buffer
			ImGui::TextWrapped("%s", textBuffer.begin(), textBuffer.end());

			ImGui::PopTextWrapPos();

			//scrolls to the bottom if scrolling is allowed
			//and if user is close to the newest console message
			if (allowScrollToBottom)
			{
				bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
				if (isNearBottom
					|| (!firstScrollToBottom
						&& Engine::startedWindowLoop))
				{
					ImGui::SetScrollHereY(1.0f);
					firstScrollToBottom = true;
				}
			}

			AddTextToConsole();

			ImGui::EndChild();

			//text filter input box
			float textAreaHeight = ImGui::GetContentRegionAvail().y - 25.0f;
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + textAreaHeight));

			// Draw the text filter input box
			ImGui::PushItemWidth(scrollingRegionSize.x);
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackCharFilter;
			ImGui::InputText("##filter", textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), flags,
				[](ImGuiInputTextCallbackData* data)
				{
					char c = data->EventChar;
					if (c == '\n' || c == '\r')
					{
						return 1;
					}
					return 0;
				});

			//retrieve the text entered by the user
			const char* filterText = textFilter.InputBuf;

			//check if Enter/Return was pressed, if so, reset the processed flag and clear the input field
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))
				|| ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_KeyPadEnter)))
			{
				//parse the filter text
				if (!filterTextProcessed
					&& filterText
					&& filterText[0] != '\0')
				{
					ConsoleManager::ParseConsoleCommand(filterText);
				}

				memset(textFilter.InputBuf, 0, sizeof(textFilter.InputBuf));
			}

			ImGui::PopItemWidth();

			ImGui::End();
		}
	}
	void GUI::AddTextToConsole()
	{
		if (writeToConsole)
		{
			textBuffer.appendf("%s", addedText.c_str());

			//second scroll update that scrolls to the bottom if scrolling is allowed
			//and when any new text is added. top one fully scrolls to the bottom,
			//this one is used because there is no better way to force scroll to the bottom
			//when enter is pressed and only using this doesnt fully scroll to the bottom
			//so this works together with the top scroll update to always scroll to the bottom
			if (allowScrollToBottom) ImGui::SetScrollHereY(1.0f);

			writeToConsole = false;
		}
	}

	void GUI::RenderSceneMenu()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (showSceneMenu
			&& ImGui::Begin("Scene menu", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showSceneMenu = false;
			}
			
			/*
			ImGui::Text("Directional light angle");
			ImGui::PushItemWidth(100);
			ImGui::Text("   x   ");
			ImGui::SameLine();
			ImGui::InputFloat("##dirlightangleX", &Render::directionalLightAngle.x);
			Render::directionalLightAngle.x = clamp(Render::directionalLightAngle.x, -1.0f, 1.0f);
			ImGui::SameLine();
			ImGui::Text("   y   ");
			ImGui::SameLine();
			ImGui::InputFloat("##dirlightangleY", &Render::directionalLightAngle.y);
			Render::directionalLightAngle.y = clamp(Render::directionalLightAngle.y, -1.0f, 1.0f);
			ImGui::SameLine();
			ImGui::Text("   z   ");
			ImGui::SameLine();
			ImGui::InputFloat("##dirlightangleZ", &Render::directionalLightAngle.z);
			Render::directionalLightAngle.z = clamp(Render::directionalLightAngle.z, -1.0f, 1.0f);
			ImGui::PopItemWidth();
			*/

			ImGui::Text("Light linear");
			ImGui::SliderFloat("##lightLin", &Render::lightLinear, 0.0f, 1.0f);
			ImGui::SameLine();
			if (ImGui::Button("Reset##lightLin"))
			{
				Render::lightLinear = 0.09f;
			}
			ImGui::Text("Light quadratic");
			ImGui::SliderFloat("##lightQuad", &Render::lightQuadratic, 0.0f, 1.0f);
			ImGui::SameLine();
			if (ImGui::Button("Reset##lightQuad"))
			{
				Render::lightQuadratic = 0.032f;
			}
			ImGui::Separator();

			ImGui::Text("Background color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 45);
			ImGui::ColorButton("Background color", bgrColor);
			RWPart_BackgroundColor();
			
			ImGui::Text("Cube color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 45);
			ImGui::ColorButton("Cube color", cubeColor);
			RWPart_CubeColor();
			
			ImGui::Text("Specular color");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 45);
			ImGui::ColorButton("Specular color", specularColor);
			RWPart_SpecularColor();
			
			ImGui::Text("Cube color intensity");
			ImGui::SliderFloat("##cubecolint", &Render::cubeColorStrength, 0.0f, 1.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how intense the cube color should be.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##cubecolint"))
			{
				Render::cubeColorStrength = 0.5f;
			}

			ImGui::Text("Ambient color intensity");
			ImGui::SliderFloat("##ambcolint", &Render::ambientColorStrength, 0.0f, 1.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how intense the ambient color should be.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##ambcolint"))
			{
				Render::ambientColorStrength = 0.2f;
			}

			ImGui::Text("Shininess");
			ImGui::SliderFloat("##shininess", &Render::shininess, 3.0f, 128.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how intense the material shininess should be.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##shininess"))
			{
				Render::shininess = 32.0f;
			}
			
			ImGui::Separator();

			ImGui::Text("Cube speed multiplier");
			ImGui::SliderFloat("##cubespeedmult", &Render::cubeSpeedMultiplier, 0.0f, 0.1f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust the overall cube speed multiplier.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##cubespeedmult"))
			{
				Render::cubeSpeedMultiplier = 0.005f;
			}

			ImGui::Text("Lamp orbit range");
			ImGui::SliderFloat("##lamporbitrange", &Render::lampOrbitRange, 2.0f, 10.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how far the cube should orbit from the lamp.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##lamporbitrange"))
			{
				Render::lampOrbitRange = 5.0f;
			}

			ImGui::Text("Cube wiggle height");
			ImGui::SliderFloat("##cubewheight", &Render::cubeWiggleHeight, 0.0f, 5.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how high and low the cube should wiggle.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##cubewheight"))
			{
				Render::cubeWiggleHeight = 2.0f;
			}

			ImGui::Text("Cube wiggle speed");
			ImGui::SliderFloat("##cubewspeed", &Render::cubeWiggleSpeed, 0.0f, 10.0f);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Adjust how fast the cube should wiggle.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##cubewspeed"))
			{
				Render::cubeWiggleSpeed = 1.0f;
			}

			ImGui::End();
		}
	}
	
	void GUI::RWPart_BackgroundColor()
	{
		if (ImGui::IsItemClicked())
		{
			ImGui::OpenPopup("Background color");
		}

		if (ImGui::BeginPopup("Background color"))
		{
			if (ImGui::ColorPicker4("##bgrcol", (float*)&bgrColor))
			{
				Render::backgroundColor.x = bgrColor.x;
				Render::backgroundColor.y = bgrColor.y;
				Render::backgroundColor.z = bgrColor.z;
			}

			ImGui::EndPopup();
		}
	}
	void GUI::RWPart_CubeColor()
	{
		if (ImGui::IsItemClicked())
		{
			ImGui::OpenPopup("Cube color");
		}

		if (ImGui::BeginPopup("Cube color"))
		{
			if (ImGui::ColorPicker3("##cubecol", (float*)&cubeColor))
			{
				Render::cubeColor.x = cubeColor.x;
				Render::cubeColor.y = cubeColor.y;
				Render::cubeColor.z = cubeColor.z;
			}

			ImGui::EndPopup();
		}
	}
	void GUI::RWPart_SpecularColor()
	{
		if (ImGui::IsItemClicked())
		{
			ImGui::OpenPopup("Specular color");
		}

		if (ImGui::BeginPopup("Specular color"))
		{
			if (ImGui::ColorPicker3("##speccol", (float*)&specularColor))
			{
				Render::specularColor.x = specularColor.x;
				Render::specularColor.y = specularColor.y;
				Render::specularColor.z = specularColor.z;
			}

			ImGui::EndPopup();
		}
	}

	void GUI::RenderVersionCheckWindow()
	{
		ImVec2 initialPos(400, 200);
		ImVec2 initialSize(400, 400);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;

		if (showVersionWindow
			&& ImGui::Begin("Version", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				showVersionWindow = false;
			}

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(versionCompare.c_str()).x) * 0.48f);
			ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::CalcTextSize(versionCompare.c_str()).y) * 0.35f);
			ImGui::Text(versionCompare.c_str());

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(versionConfirm.c_str()).x) * 0.48f);
			ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::CalcTextSize(versionConfirm.c_str()).y) * 0.45f);
			ImGui::Text(versionConfirm.c_str());

			if (outdatedVersion)
			{
				ImVec2 buttonSize(200, 50);

				//move button to center and slightly down
				ImVec2 buttonPos
				(
					(ImGui::GetWindowWidth() - buttonSize.x) * 0.5f,
					(ImGui::GetWindowHeight() - buttonSize.y) * 0.7f
				);

				ImGui::SetCursorPos(buttonPos);

				if (ImGui::Button("Get the latest version", buttonSize))
				{
					try
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::DEBUG,
							"User opened link to Github repository releases page.\n");
						Browser::OpenLink("https://github.com/Lost-Empire-Entertainment/Elypso-engine/releases");
					}
					catch (const exception& e)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::EXCEPTION,
							"Error: Failed to open link to Github repository releases page! " + string(e.what()) + "\n");
					}
				}
			}

			ImGui::End();
		}
	}

	void GUI::RWPart_DebugMenuInfo()
	{
		ImGui::Text("FPS: %.2f", TimeManager::displayedFPS);
		ImGui::Text(
			"Position: %.2f, %.2f, %.2f",
			Render::cameraPos.x,
			Render::cameraPos.y,
			Render::cameraPos.z);
		ImGui::Text(
			"Angle: %.2f, %.2f, %.2f",
			Render::camera.GetCameraRotation().x,
			Render::camera.GetCameraRotation().y,
			Render::camera.GetCameraRotation().z);
		ImGui::Text("FOV: %.0f", Graphics::Render::fov);
	}
	void GUI::RWPart_Interactions()
	{
		ImGui::Text("Debug buttons");
		ImGui::Text("");

		ImGui::Text("Enable VSync");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		if (ImGui::Checkbox("##vsync", &Render::useMonitorRefreshRate))
		{
			glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);
		}

		ImGui::Text("Enable FPS messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##fpsmsg", &Input::inputSettings.printFPSToConsole);

		ImGui::Text("Enable ImGui messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##imguimsg", &Input::inputSettings.printIMGUIToConsole);

		ImGui::Text("Enable input messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##inputmsg", &Input::inputSettings.printInputToConsole);

		ImGui::Text("Enable console scroll to bottom");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##consolescroll", &allowScrollToBottom);

		ImGui::Text("Enable console debug messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##consoledebugmsg", &ConsoleManager::sendDebugMessages);

		//
		//CAMERA CLIP RANGE
		//

		ImGui::Separator();

		ImGui::Text("Camera clip range");
		ImGui::Text("");

		ImGui::Text("Near clip");
		ImGui::SliderFloat("##nearclip", &Render::nearClip, 0.001f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera near clip range.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##nearclip"))
		{
			Render::nearClip = 0.001f;
		}

		ImGui::Text("Far clip");
		ImGui::SliderFloat("##farclip", &Render::farClip, 10.0f, 100.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera far clip range.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##farclip"))
		{
			Render::farClip = 100.0f;
		}

		//
		//MOVE SPEED MULTIPLIER
		//

		ImGui::Separator();

		ImGui::Text("Move speed multiplier");
		ImGui::Text("");
		ImGui::SliderFloat("##movespeed", &Input::inputSettings.moveSpeedMultiplier, 0.1f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera move speed.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##movespeedmult"))
		{
			Input::inputSettings.moveSpeedMultiplier = 1.0f;
		}

		//
		//FOV
		//

		ImGui::Separator();

		ImGui::Text("FOV");
		ImGui::Text("");
		ImGui::SliderFloat("##fov", &Render::fov, 70.0f, 110.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera field of view.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fov"))
		{
			Render::fov = 90.0f;
		}
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
}