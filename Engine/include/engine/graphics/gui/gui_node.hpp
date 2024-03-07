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

#include <string>
#include <memory>

//external
#include "glm.hpp"

using std::string;
using std::shared_ptr;
using glm::vec2;

namespace Graphics::GUI
{
	class Node
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		Node(
			const vec2& pos,
			const vec2& scale,
			string& name,
			unsigned int& ID) :
			pos(pos),
			scale(scale),
			name(name),
			ID(ID){}

		static shared_ptr<Node> InitializeNode(
			const vec2& pos = vec2(0),
			const vec2& scale = vec2(0),
			string& name = tempName,
			unsigned int& id = tempID);

		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		void SetPos(const vec2& newPos) { pos = newPos; }

		string GetName() const { return name; }
		unsigned int GetID() const { return ID; }
		vec2 GetPos() const { return pos; }

	private:
		vec2 pos;
		vec2 scale;
		string name;
		unsigned int ID;
	};
}