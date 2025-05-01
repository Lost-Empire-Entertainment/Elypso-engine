//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <iomanip>
#include <sstream>

//external
#include "glm.hpp"

//engine
#include "gui_game.hpp"
#include "rigidbody.hpp"

//game
#include "g_ui.hpp"
#include "g_player.hpp"

using std::string;
using std::to_string;
using glm::vec3;
using std::fixed;
using std::setprecision;
using std::ostringstream;

using Graphics::GUI::GameGUI;
using Graphics::Components::RigidBodyComponent;

namespace GameTemplate
{
	static string FormatFloat(float value)
	{
		ostringstream oss;

		oss << fixed << setprecision(2) << value;

		return oss.str();
	}

	void G_UI::AddWindowsToList()
	{
		GameGUI::imguiWindows.push_back(DebugWindow);
	}

	void G_UI::DebugWindow()
	{
		ImVec2 windowSize = ImVec2(300.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

		ImVec2 windowPos = ImVec2(20.0f, 20.0f);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (ImGui::Begin("##gameDebugWindow", NULL, windowFlags))
		{
			ImGui::Text("walk speed");
			ImGui::SameLine();
			float walkSpeed = G_Player::walkSpeed;
			if (ImGui::DragFloat("##pWalkSpeed", &walkSpeed, 0.01f, 0.1f, 10.0f))
			{
				G_Player::walkSpeed = walkSpeed;
			}

			ImGui::Text("speed amplify");
			ImGui::SameLine();
			float speedAmplify = G_Player::speedAmplify;
			if (ImGui::DragFloat("##pSpeedAmplify", &speedAmplify, 0.1f, 1.0f, 50.0f))
			{
				G_Player::speedAmplify = speedAmplify;
			}

			ImGui::Text("jump strength");
			ImGui::SameLine();
			float jumpStrength = G_Player::jumpStrength;
			if (ImGui::DragFloat("##pJumpStrength", &jumpStrength, 0.1f, 0.1f, 10.0f))
			{
				G_Player::jumpStrength = jumpStrength;
			}

			ImGui::Text("max velocity");
			ImGui::SameLine();
			float maxVelocity = G_Player::maxVelocity;
			if (ImGui::DragFloat("##pMaxVelocity", &maxVelocity, 0.1f, 0.1f, 10.0f))
			{
				G_Player::maxVelocity = maxVelocity;
			}

			if (G_Player::model_player != nullptr)
			{
				ImGui::Text("");
				ImGui::Text("==============================");
				ImGui::Text("");

				static shared_ptr<RigidBodyComponent> mrc{};
				if (mrc == nullptr)
				{
					mrc = G_Player::model_player->GetComponent<RigidBodyComponent>();
				}

				vec3 playerVelocity = mrc->GetVelocity();

				string vX = FormatFloat(playerVelocity.x);
				string vY = FormatFloat(playerVelocity.y);
				string vZ = FormatFloat(playerVelocity.z);

				ImGui::Text("current velocity");
				ImGui::Text(("x: " + vX).c_str());
				ImGui::Text(("y: " + vY).c_str());
				ImGui::Text(("z: " + vZ).c_str());
			}

			ImGui::Text("");
			ImGui::Text("==============================");
			ImGui::Text("");

			ImGui::Text("Walk: WASD");
			ImGui::Text("Jump: Space");
			ImGui::Text("Sprint: Left Shift");
			ImGui::Text("Lock/Unlock cursor: ESC");

			ImGui::End();
		}
	}
}