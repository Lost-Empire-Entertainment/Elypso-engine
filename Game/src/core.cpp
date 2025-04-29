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
#include "core.hpp"
#include "gui.hpp"
#include "input.hpp"

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
	static bool calledModelCheckOnce = false;
	static shared_ptr<GameObject> model = nullptr;
	static string modelName = "";

	void T_Core::Initialize()
	{
		Game_Core::Initialize();
		T_UI::AddWindowsToList();
		T_Input::Initialize();
	}

	void T_Core::Run()
	{
		while (!Game_Core::ShouldClose())
		{
			Game_Core::Run();

			ModelCheck("Point light");
			KeyCheck();
		}
	}

	void T_Core::Shutdown()
	{
		Game_Core::Shutdown();
	}

	void T_Core::ModelCheck(const string& name)
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

	void T_Core::KeyCheck()
	{
		T_Input::UpdateKeyStates();
		if (T_Input::IsPressed("R"))
		{
			if (model == nullptr) return;

			float min = 0.0f;
			float max = 1.0f;

			float R = RandomFloat(min, max);
			float G = RandomFloat(min, max);
			float B = RandomFloat(min, max);

			SetLightColor(model, R, G, B);
		}
	}
	float T_Core::RandomFloat(float min, float max)
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dist(min, max);

		return dist(gen);
	}
	void T_Core::SetLightColor(
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
