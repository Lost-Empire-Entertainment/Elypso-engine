//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

#include "gameobject.hpp"

using std::shared_ptr;

using Graphics::Shape::GameObject;

namespace Graphics::GUI
{
	class GUISceneHierarchy
	{
	public:
		static inline bool renderSceneHierarchy;
		static void RenderSceneHierarchy();
	private:
		static inline char newNameBuffer[64];
		static inline bool isRenaming;
		static inline shared_ptr<GameObject> copiedGameObject;
		static inline shared_ptr<GameObject> selectedGameObject;

		static void DrawGameObject(shared_ptr<GameObject> obj);
		static void RightClickPopup();
		static void RenameGameObject();
	};
}