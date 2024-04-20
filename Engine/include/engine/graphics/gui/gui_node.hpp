//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>
#include <vector>

//external
#include "glm.hpp"
#include "gui_nodecircle.hpp"

using std::string;
using std::shared_ptr;
using std::vector;
using glm::vec2;

namespace Graphics::GUI
{
	class GUINode
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		GUINode(
			const vec2& pos,
			const vec2& initialPos,
			const vec2& scale,
			string& name,
			unsigned int& ID,
			const vector<shared_ptr<GUINodeCircle>> nodeCircles) :
			pos(initialPos),
			initialPos(initialPos),
			scale(scale),
			name(name),
			ID(ID),
			nodeCircles(nodeCircles){}

		static shared_ptr<GUINode> InitializeNode(
			const vec2& pos,
			const vec2& scale,
			string& name,
			unsigned int&,
		    const vector<shared_ptr<GUINodeCircle>> nodeCircles);

		void SetPos(const vec2& newPos) { pos = newPos; }
		void SetInitialPos(const vec2& newInitialPos) { initialPos = newInitialPos; }
		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		
		vec2 GetPos() const { return pos; }
		vec2 GetInitialPos() const { return initialPos; }
		string GetName() const { return name; }
		unsigned int GetID() const { return ID; }

		vector<shared_ptr<GUINodeCircle>>& GetNodeCircles() { return nodeCircles; }
		
	private:
		vec2 pos;
		vec2 initialPos;
		vec2 scale;
		string name;
		unsigned int ID;
		vector<shared_ptr<GUINodeCircle>> nodeCircles;
		vector<shared_ptr<GUINode>> connectedNodes;
	};
}