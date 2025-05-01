//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "glm.hpp"

//engine
#include "console.hpp"
#include "importer.hpp"
#include "gameobject.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"

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
using Graphics::Shape::Importer;
using Graphics::Shape::GameObject;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;

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
		string acDesc = "Add a simple 1x1x1 cube in front of the player, you must pass string as first parameter for cube name.";
		ConsoleManager::AddCommand("ac", acDesc, 2, CommandTarget::TARGET_GAME, CreateCube);

		string rdwDesc = "Set debug window render state (1 = on, 0 = off).";
		ConsoleManager::AddCommand("rdw", rdwDesc, 2, CommandTarget::TARGET_GAME, RenderDebugWindow);
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

	void G_Console::CreateCube(const vector<string>& args)
	{
		auto ptc = G_Player::model_player->GetComponent<TransformComponent>();
		vec3 pos = ptc->GetPosition();
		vec3 rot = ptc->GetRotation();
		vec3 scale = vec3(0.2f);

		string cubeModelPath = path(
			current_path()
			/ "project"
			/ "scenes"
			/ "Scene1"
			/ "gameobjects"
			/ "floor"
			/ "floor.fbx").string();

		if (!exists(cubeModelPath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to find cube model at path '" + cubeModelPath + "'!\n");

			return;
		}

		string name = args[1];
		unsigned int newID = ++customID;

		Importer::Initialize(
			pos,
			rot,
			scale,
			cubeModelPath,
			"DEFAULTDIFF",
			"DEFAULTSPEC",
			"EMPTY",
			"EMPTY",
			false,
			1.0f,
			32.0f,
			name,
			newID,
			true);

		string posStr =
			"(" +
			to_string(pos.x) + ", " +
			to_string(pos.y) + ", " +
			to_string(pos.z) + ")";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"Successfully created new cube model with name '" 
			+ name + "' at position " + posStr + "!\n");
	}
}