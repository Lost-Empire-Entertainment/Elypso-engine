//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "glm.hpp"

//engine
#include "console.hpp"
#include "transformcomponent.hpp"
#include "rigidbodycomponent.hpp"
#include "physics.hpp"

//game
#include "g_console.hpp"
#include "g_player.hpp"
#include "g_states.hpp"

using glm::vec3;
using std::filesystem::exists;
using std::filesystem::current_path;
using std::filesystem::path;
using std::to_string;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::ConsoleManager;
using Core::CommandTarget;
using Graphics::Components::TransformComponent;
using Graphics::Components::RigidBodyComponent;
using Core::Physics;

namespace GameTemplate
{
	static unsigned int customID = 100000;

	static bool StringToInt(const string& in, int& out)
	{
		try
		{
			size_t pos{};
			out = stoi(in, &pos);
			return pos == in.length();
		}
		catch (...)
		{
			return false;
		}
	}

	void G_Console::AddCommands()
	{
		string rdwDesc = "Set debug window render state (1 = on, 0 = off).";
		ConsoleManager::AddCommand("rdw", rdwDesc, 2, CommandTarget::TARGET_GAME, RenderDebugWindow);

		string rpDesc = "Attempts to free the player if stuck in the ground or any object.";
		ConsoleManager::AddCommand("rp", rpDesc, 1, CommandTarget::TARGET_GAME, ResetPlayer);
	}

	void G_Console::RenderDebugWindow(const vector<string>& args)
	{
		int value{};

		if (!StringToInt(args[1], value))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Parameter '" + args[1] + "' is not an integer! Type 'h rdw' for more info.\n");

			return;
		}

		if (value != 0
			&& value != 1)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Parameter '" + args[1] + "' is out of range! Type 'h rdw' for more info.\n");

			return;
		}

		G_States::renderDebugWindow = value;

		string valueStr = value == 1 ? "true" : "false";
		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			"Successfully set 'render debug window' state to '" + valueStr + "'!\n");
	}

	void G_Console::ResetPlayer(const vector<string>& args)
	{
		auto mtc = G_Player::model_player->GetComponent<TransformComponent>();
		auto mrc = G_Player::model_player->GetComponent<RigidBodyComponent>();
		auto ctc = G_Player::camera->GetComponent<TransformComponent>();

		vec3 startPos = mtc->GetPosition();
		startPos = startPos + vec3(0.0f, 2.0f, 0.0f);

		Physics::simulatePhysics = false;

		mtc->SetPosition(startPos);
		ctc->SetPosition(startPos);

		mrc->ResetVelocity();
		mrc->ResetAngularVelocity();

		Physics::simulatePhysics = true;
	}
}