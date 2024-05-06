//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//engine
#include "gui_node.hpp"
#include "gui_nodeblock.hpp"
#include "gameobject.hpp"

using std::cout;
using std::to_string;
using std::make_shared;

using Graphics::Shape::GameObject;

namespace Graphics::GUI
{
	shared_ptr<GUINode> GUINode::InitializeNode(
		const vec2& pos,
		const vec2& scale,
		string& name,
		unsigned int& id,
		const vector<shared_ptr<GUINodeCircle>> nodeCircles)
	{
		if (name == tempName) name = "Node";
		if (id == tempID) id = GameObject::nextID++;

		shared_ptr<GUINode> node = make_shared<GUINode>(
			pos,
			pos,
			scale,
			name,
			id,
			nodeCircles);

		return node;
	}
}