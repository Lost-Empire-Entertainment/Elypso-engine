//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>

//engine
#include "compile.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "gameobject.hpp"
#include "gui_node.hpp"
#include "gui_nodecircle.hpp"
#include "gui_nodeconnection.hpp"

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::exception;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::GUINode;
using Graphics::GUI::GUINodeCircle;
using Graphics::GUI::GUINodeConnection;
using Graphics::Shape::Mesh;
using Graphics::Shape::Component;

namespace Core
{
	void Compilation::Compile()
	{
		try
		{
			if (Engine::gamePath == ""
				|| Engine::gameExePath == ""
				|| Engine::gameParentPath == "")
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Game path has not been set!\n");
			}
			else
			{
				string gameBatPath = Engine::gamePath + "\\build_engine_fast.bat";
				gameBatPath = String::CharReplace(gameBatPath, '/', '\\');

				int result = File::RunBatFile(gameBatPath);

				if (result != 0)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Compilation failed! Trying again...\n");

					string gameBatPath = Engine::gamePath + "\\build_engine_full.bat";
					gameBatPath = String::CharReplace(gameBatPath, '/', '\\');

					int result = File::RunBatFile(gameBatPath);

					if (result != 0)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Compilation failed again! Something is broken, please contact engine developers...\n");

						return;
					}
				}

				string gameProjectFolder = Engine::gameParentPath + "\\files\\project";
				for (const auto& item : directory_iterator(path(gameProjectFolder)))
				{
					if (is_directory(item.path())
						|| is_regular_file(item.path()))
					{
						string itemPath = item.path().string();

						File::DeleteFileOrfolder(itemPath);
					}
				}

				for (const auto& obj : GameObjectManager::GetObjects())
				{
					if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::model)
					{
						for (const auto& component : obj->GetComponents())
						{
							if (component->GetType() == Component::ComponentType::Nodeblock)
							{
								string objName = obj->GetName();
								size_t nodeCount = component->GetNodes().size();
								size_t nodeConnectionCount = component->GetNodeConnections().size();

								cout << objName << " has " 
									<< nodeCount << " nodes and "
									<< nodeConnectionCount << " node connections...\n";
							}
						}
					}
				}

				string engineProjectPath = path(Engine::filesPath).parent_path().string() + "\\project";
				for (const auto& item : directory_iterator(path(engineProjectPath)))
				{
					string itemPath = item.path().string();

					string ending = item.is_directory()
						? path(item).filename().string()
						: path(item).stem().string() + path(item).extension().string();
					string targetItemPath = gameProjectFolder + "\\" + ending;

					File::CopyFileOrFolder(itemPath, targetItemPath);
				}

				File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			}
		}
		catch (const exception& e)
		{
			cout << e.what() << "\n";
		}
	}
}