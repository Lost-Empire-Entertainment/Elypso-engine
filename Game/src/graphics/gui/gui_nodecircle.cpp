//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "gui_nodecircle.hpp"
#include "gameobject.hpp"

using Graphics::Shape::GameObject;

namespace Graphics::GUI
{
	shared_ptr<GUINodeCircle> GUINodeCircle::InitializeNodeCircle(
		const vec2& pos,
		string name,
		unsigned int ID,
		const Side& side,
		const Slot& slot)
	{
		if (name == tempName) name = "NodeCircle";
		if (ID == tempID) ID = GameObject::nextID++;

		shared_ptr<GUINodeCircle> nodeCircle = make_shared<GUINodeCircle>(
			pos,
			pos,
			name,
			ID,
			side,
			slot);

		return nodeCircle;
	}
}