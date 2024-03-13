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
#include "imgui.h"

#include "gui_nodecircle.hpp"

using std::string;
using std::shared_ptr;

namespace Graphics::GUI
{
	class GUINodeConnection
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		GUINodeConnection(
			string name,
			unsigned int ID,
			const shared_ptr<GUINodeCircle>& curveStart,
			const shared_ptr<GUINodeCircle>& curveEnd) :
			name(name),
			ID(ID),
			curveStart(curveStart),
			curveEnd(curveEnd) {}

		static shared_ptr<GUINodeConnection> InitializeNodeConnection(
			string name,
			unsigned int ID,
			const shared_ptr<GUINodeCircle>& curveStart,
			const shared_ptr<GUINodeCircle>& curveEnd);

		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		void SetCurveStart(const  shared_ptr<GUINodeCircle>& newCurveStart) { curveStart = newCurveStart; }
		void SetCurveEnd(const  shared_ptr<GUINodeCircle>& newCurveEnd) { curveEnd = newCurveEnd; }
		
		string GetName() const { return name; }
		unsigned int GetID() const { return ID; }
		shared_ptr<GUINodeCircle> GetCurveStart() const { return curveStart; }
		shared_ptr<GUINodeCircle> GetCurveEnd() const { return curveEnd; }

	private:
		string name;
		unsigned int ID;
		shared_ptr<GUINodeCircle> curveStart;
		shared_ptr<GUINodeCircle> curveEnd;
	};
}