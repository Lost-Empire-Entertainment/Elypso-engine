//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

//external
#include "imgui.h"
#include "glfw3.h"

//engine
#include "gameobject.hpp"
#include "gui_node.hpp"

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