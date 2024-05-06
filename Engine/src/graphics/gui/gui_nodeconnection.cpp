//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "gui_nodeconnection.hpp"
#include "gameobject.hpp"

using Graphics::Shape::GameObject;
using Graphics::Shape::Component;

namespace Graphics::GUI
{
	shared_ptr<GUINodeConnection> GUINodeConnection::InitializeNodeConnection(
		string name,
		unsigned int ID,
		const shared_ptr<GUINodeCircle>& curveStart,
		const shared_ptr<GUINodeCircle>& curveEnd)
	{
		if (name == tempName) name = "NodeConnection";
		if (ID == tempID) ID = GameObject::nextID++;

		shared_ptr<GUINodeConnection> nodeConnection = make_shared<GUINodeConnection>(
			name,
			ID,
			curveStart,
			curveEnd);

		return nodeConnection;
	}
}