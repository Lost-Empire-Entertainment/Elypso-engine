//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <random>
#include <sstream>
#include <unordered_map>

//external
#include "magic_enum.hpp"
#include "glfw3.h"
#include "glm.hpp"

//engine
#include "game_core.hpp"
#include "game_asset.hpp"
#include "console.hpp"
#include "lightcomponent.hpp"
#include "meshcomponent.hpp"
#include "render.hpp"

//game
#include "g_core.hpp"
#include "g_ui.hpp"
#include "g_input.hpp"

using std::string;
using std::shared_ptr;
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
using glm::vec3;
using std::ostringstream;
using std::unordered_map;

using Game::Game_Core;
using Game::Game_Asset;
using Core::ConsoleManager;
using Graphics::Components::LightComponent;
using Graphics::Components::MeshComponent;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Render;

namespace GameTemplate
{
	static bool currentYMovementState = false;
	static bool currentRightClickState = false;
	static bool currentMovementState = true;

	static bool calledModelCheckOnce = false;
	static shared_ptr<GameObject> model = nullptr;
	static string modelName = "";

	void G_Core::Initialize()
	{
		Game_Core::Initialize();
		G_UI::AddWindowsToList();
		G_Input::Initialize();

		Game_Core::SetYMovementState(currentYMovementState);
		Game_Core::SetRightClickState(currentRightClickState);
		Game_Core::SetMovementState(currentMovementState);
	}

	void G_Core::Run()
	{
		while (!Game_Core::ShouldClose())
		{
			Game_Core::Run();

			ModelCheck("Point light");
			KeyCheck();
		}
	}

	void G_Core::Shutdown()
	{
		Game_Core::Shutdown();
	}

	void G_Core::ModelCheck(const string& name)
	{
		if (!calledModelCheckOnce)
		{
			model = Game_Asset::GetGameObject(name);
			if (model != nullptr)
			{
				modelName = name;
				MeshComponent::MeshType type = model->GetComponent<MeshComponent>()->GetMeshType();
				string meshType = string(magic_enum::enum_name(type));
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::INFO,
					"Successfully found model with name '" + name + "' and mesh type '" + meshType + "'!");
			}
			else
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Did not find model with name '" + name + "'!");
			}
			calledModelCheckOnce = true;
		}
	}

	void G_Core::KeyCheck()
	{
		G_Input::UpdateKeyStates();
		if (G_Input::IsPressed("R"))
		{
			if (model == nullptr) return;

			float min = 0.0f;
			float max = 1.0f;

			float R = RandomFloat(min, max);
			float G = RandomFloat(min, max);
			float B = RandomFloat(min, max);

			SetLightColor(model, R, G, B);
		}

		if (G_Input::IsPressed("Z"))
		{
			currentYMovementState = !currentYMovementState;
			Game_Core::SetYMovementState(currentYMovementState);
		}
		if (G_Input::IsPressed("X"))
		{
			currentRightClickState = !currentRightClickState;
			Game_Core::SetRightClickState(currentRightClickState);
		}
		if (G_Input::IsPressed("C"))
		{
			currentMovementState = !currentMovementState;
			Game_Core::SetMovementState(currentMovementState);

		}
	}
	float G_Core::RandomFloat(float min, float max)
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dist(min, max);

		return dist(gen);
	}
	void G_Core::SetLightColor(
		shared_ptr<GameObject> obj,
		float r,
		float g,
		float b)
	{
		if (model != nullptr)
		{
			shared_ptr<LightComponent> lc = model->GetComponent<LightComponent>();
			if (lc == nullptr)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Did not find point light component from model with name '" + modelName + "'!");
			}
			else
			{
				lc->SetDiffuse(vec3(r, g, b));

				ostringstream oss;
				oss << "("
					<< r
					<< ", "
					<< g
					<< ", "
					<< b
					<< ")";

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"Successfully set point light color to " + oss.str() + "!\n");
			}
		}
	}
}
