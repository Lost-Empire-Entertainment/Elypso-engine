//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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