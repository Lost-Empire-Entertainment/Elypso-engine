//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <iomanip>
#include <sstream>

//external
#include "glm.hpp"
#include "core/rigidbody.hpp"

//engine
#include "gui_game.hpp"
#include "transformcomponent.hpp"
#include "cameracomponent.hpp"

//game
#include "g_ui.hpp"
#include "g_player.hpp"
#include "g_states.hpp"

using std::string;
using std::to_string;
using glm::vec3;
using std::fixed;
using std::setprecision;
using std::ostringstream;

using Graphics::GUI::GameGUI;
using Graphics::Components::RigidBodyComponent;
using Graphics::Components::TransformComponent;
using Graphics::Components::CameraComponent;

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

		float height = G_States::isConsoleOpen ? 420.0f : 20.0f;
		ImVec2 windowPos = ImVec2(20.0f, height);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (G_States::renderDebugWindow
			&& ImGui::Begin("##gameDebugWindow", NULL, windowFlags))
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

				//display player position

				static shared_ptr<TransformComponent> mtc{};
				if (mtc == nullptr)
				{
					mtc = G_Player::model_player->GetComponent<TransformComponent>();
				}

				vec3 playerPosition = mtc->GetPosition();

				string pX = FormatFloat(playerPosition.x);
				string pY = FormatFloat(playerPosition.y);
				string pZ = FormatFloat(playerPosition.z);

				ImGui::Text(("current position: (" + pX + ", " + pY + ", " + pZ + ")").c_str());

				//display player rotation

				static shared_ptr<CameraComponent> mcc{};
				if (mcc == nullptr)
				{
					mcc = G_Player::camera->GetComponent<CameraComponent>();
				}

				vec3 playerRotation = mcc->GetFront();

				string rX = FormatFloat(playerRotation.x);
				string rY = FormatFloat(playerRotation.y);
				string rZ = FormatFloat(playerRotation.z);

				ImGui::Text(("current rotation: (" + rX + ", " + rY + ", " + rZ + ")").c_str());

				//display player velocity

				static shared_ptr<RigidBodyComponent> mrc{};
				if (mrc == nullptr)
				{
					mrc = G_Player::model_player->GetComponent<RigidBodyComponent>();
				}

				vec3 playerVelocity = mrc->GetVelocity();

				string vX = FormatFloat(playerVelocity.x);
				string vY = FormatFloat(playerVelocity.y);
				string vZ = FormatFloat(playerVelocity.z);

				ImGui::Text(("current velocity: (" + vX + ", " + vY + ", " + vZ + ")").c_str());
			}

			ImGui::Text("");
			ImGui::Text("==============================");
			ImGui::Text("");

			ImGui::Text("Walk: WASD");
			ImGui::Text("Jump: Space");
			ImGui::Text("Sprint: Left Shift");
			ImGui::Text("Lock/Unlock cursor: ESC");
			ImGui::Text("Open/close console: Page up");

			ImGui::End();
		}
	}
}