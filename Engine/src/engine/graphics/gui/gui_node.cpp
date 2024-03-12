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