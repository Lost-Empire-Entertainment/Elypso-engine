//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>

//external
#include "magic_enum.hpp"

//engine
#include "nodeBlockFile.hpp"
#include "fileUtils.hpp"
#include "sceneFile.hpp"
#include "console.hpp"
#include "gui_nodecircle.hpp"
#include "gui_nodeconnection.hpp"
#include "gui_node.hpp"

using std::ifstream;
using std::ofstream;
using std::filesystem::path;
using std::filesystem::exists;

using Utils::File;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::GUI::GUINode;
using Graphics::GUI::GUINodeCircle;
using Graphics::GUI::GUINodeConnection;

namespace EngineFile
{
	void NodeBlockFile::LoadNodeBlock(const shared_ptr<GameObject> target, const string& nodeBlockFilePath)
	{

	}

	string NodeBlockFile::SaveNodeBlock(const shared_ptr<Component> component, const shared_ptr<GameObject> target)
	{
		string output;

		for (const auto& node : component->GetNodes())
		{
			output += "node_" + node->GetName() + "_" 
				+ to_string(node->GetID()) + "\n";

			for (const auto& nodeCircle : node->GetNodeCircles())
			{
				output += "    nodeCircle_" + to_string(nodeCircle->GetID()) + "\n";
				output += "        side= " + string(magic_enum::enum_name(nodeCircle->GetSide())) + "\n";
				output += "        slot= " + string(magic_enum::enum_name(nodeCircle->GetSlot())) + "\n";

				if (nodeCircle->GetNodeConnections().size() == 0)
				{
					output += "        NO CONNECTIONS IN NODE CIRCLE\n\n";
				}
				else
				{
					for (const auto& nodeConnection : nodeCircle->GetNodeConnections())
					{
						output += "        nodeConnection_" + to_string(nodeConnection->GetID()) + "\n";

						shared_ptr<GUINodeCircle> startNodeCircle = nodeConnection->GetCurveStart();
						string startNodeCircleName = startNodeCircle != nullptr
							? "nodeCircle_" + to_string(startNodeCircle->GetID())
							: "NONE";
						string startNodeName = startNodeCircle != nullptr
							? "node_" + startNodeCircle->GetParent()->GetName() + "_"
							+ to_string(startNodeCircle->GetParent()->GetID())
							: "NONE";

						shared_ptr<GUINodeCircle> endNodeCircle = nodeConnection->GetCurveEnd();
						string endNodeCircleName = endNodeCircle != nullptr
							? "nodeCircle_" + to_string(endNodeCircle->GetID())
							: "NONE";
						string endNodeName = endNodeCircle != nullptr
							? "node_" + endNodeCircle->GetParent()->GetName() + "_"
							+ to_string(endNodeCircle->GetParent()->GetID())
							: "NONE";

						output += "            startNodeCircle= " + startNodeCircleName + "\n";
						output += "            startNode= " + startNodeName + "\n";
						output += "            endNodeCircle= " + endNodeCircleName + "\n";
						output += "            endNode= " + endNodeName + "\n";

						output += "\n";
					}
				}
			}
		}

		string parentPath = path(SceneFile::currentScenePath).parent_path().string();
		string nodeBlockFilePath = parentPath + "/NodeBlock_" + component->GetName() + ".txt";

		ofstream outFile(nodeBlockFilePath);
		if (!outFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open node block file '" + nodeBlockFilePath + "'!\n");
			return "NONE";
		}
		else
		{
			outFile << output;
			outFile.close();
		}

		cout << "\n";

		return nodeBlockFilePath;
	}
}