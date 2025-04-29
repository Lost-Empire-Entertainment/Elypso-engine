//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//engine
#include "gameobject.hpp"

namespace GameTemplate
{
	using Graphics::Shape::GameObject;

	class G_Core
	{
	public:
		static void Initialize();
		static void Run();
		static void Shutdown();
	};
}