//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <memory>

#include "gameobject.hpp"

using std::string;
using std::shared_ptr;

using Graphics::Shape::GameObject;
using Graphics::Shape::Component;

namespace GameFile
{
	class NodeBlockFile
	{
	public:
		static void LoadNodeBlock(const shared_ptr<GameObject> target, const string& nodeBlockFilePath);
		static string SaveNodeBlock(const shared_ptr<Component> component, const shared_ptr<GameObject> target);
	};
}