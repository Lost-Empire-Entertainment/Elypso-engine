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

//external
#include "imgui.h"
#include "glfw3.h"

//engine
#include "gameobject.hpp"

using std::shared_ptr;

using Graphics::Shape::GameObject;
using Graphics::Shape::Component;

namespace Graphics::GUI
{
	class GUINodeBlock
	{
	public:
		static inline unsigned int textureID;
		static inline bool renderNodeBlock;
		static inline shared_ptr<GameObject> selectedGameObject;
		static inline shared_ptr<Component> selectedComponent;
		static inline shared_ptr<GUINode> selectedNode;

		static void SetBackgroundTexture();
		static void RenderNodeBlock();

		static void DrawBezierCurve();

		static void AddNode();

		static void DestroyNode(const shared_ptr<GUINode>& node);
	private:
		static inline unsigned int backgroundID;
		static inline bool wasNodeCircleSelected;
		static inline ImVec2 circleCenter = {};
		static inline ImVec2 curveStart = {};
		static inline ImVec2 curveEnd = {};
		static inline shared_ptr<GUINodeCircle> theStartCircle;
		static inline shared_ptr<GUINodeCircle> theEndCircle;
	};
}