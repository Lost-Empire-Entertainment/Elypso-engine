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