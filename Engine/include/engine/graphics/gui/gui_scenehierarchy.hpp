//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE

#include <memory>

//engine
#include "gameobject.hpp"

namespace Graphics::GUI
{
	using std::shared_ptr;

	using Graphics::Shape::GameObject;

	class GUISceneHierarchy
	{
	public:
		static void RenderSceneHierarchy();
	private:
		static void RenderGameobjects();
	};
}
#endif