//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "empty.hpp"
#include "selectobject.hpp"
#include "console.hpp"
#include "core.hpp"

using std::make_shared;
using std::to_string;

using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Empty::InitializeEmpty(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		string& name,
		unsigned int& id,
		const bool& isEnabled)
	{
		auto obj = make_shared<GameObject>(name, id, txtFilePath);
		auto& transform = obj->GetTransform();
		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scale);
		obj->SetEnableState(isEnabled);

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);

		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		if (obj->GetID() == tempID) obj->SetID(GameObject::nextID++);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}
}