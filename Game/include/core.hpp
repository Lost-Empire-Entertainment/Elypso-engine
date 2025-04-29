//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//engine
#include "gameobject.hpp"

namespace GameTemplate
{
	using std::string;
	using std::shared_ptr;

	using Graphics::Shape::GameObject;

	class T_Core
	{
	public:
		static void Initialize();
		static void Run();
		static void Shutdown();

		static void ModelCheck(const string& name);

		static void KeyCheck();

		static float RandomFloat(float min, float max);
		static void SetLightColor(
			shared_ptr<GameObject> obj,
			float r,
			float g,
			float b);
	};
}