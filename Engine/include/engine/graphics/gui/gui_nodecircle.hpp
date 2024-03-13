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

#include <iostream>
#include <memory>

//external
#include "glm.hpp"

using std::string;
using std::shared_ptr;
using glm::vec2;

namespace Graphics::GUI
{
	class GUINodeCircle
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		enum class Side
		{
			left,
			right
		};
		enum class Slot
		{
			first,
			second,
			third,
			fourth
		};

		GUINodeCircle(
			const vec2& pos,
			const vec2& initialPos,
			string name,
			unsigned int ID,
			const Side& side,
			const Slot& slot) :
			pos(initialPos),
			initialPos(initialPos),
			name(name),
			ID(ID),
			side(side),
			slot(slot) {}

		static shared_ptr<GUINodeCircle> InitializeNodeCircle(
			const vec2& pos,
			string name,
			unsigned int ID,
			const Side& side,
			const Slot& slot);

		void SetPos(const vec2& newPos) { pos = newPos; }
		void SetInitialPos(const vec2& newInitialPos) { initialPos = newInitialPos; }
		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		
		vec2 GetPos() const { return pos; }
		vec2 GetInitialPos() const { return initialPos; }
		string GetName() const { return name; }
		unsigned int GetID() const { return ID; }
		Side GetSide() const { return side; }
		Slot GetSlot() const { return slot; }

	private:
		vec2 pos;
		vec2 initialPos;
		string name;
		unsigned int ID;
		Side side;
		Slot slot;
	};
}