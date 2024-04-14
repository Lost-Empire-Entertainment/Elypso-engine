//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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