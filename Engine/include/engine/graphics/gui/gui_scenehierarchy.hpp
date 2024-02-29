//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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
		static inline bool rightMouseClicked;
		static inline shared_ptr<GameObject> copiedGameObject;
		static inline shared_ptr<GameObject> selectedGameObject;

		static void DrawGameObject(shared_ptr<GameObject> obj, bool isRoot);
		static void RightClickPopup();
		static void RenameGameObject();
	};
}