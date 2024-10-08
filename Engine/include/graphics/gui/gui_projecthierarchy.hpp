//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Graphics::GUI
{
	struct DirectoryEntry
	{
		string name;
		string path;
	};

	class GUIProjectHierarchy
	{
	public:
		static void RenderProjectHierarchy();
	private:
		static inline bool showPathTooltip;

		static vector<DirectoryEntry> GetDirectoryContents(const string& directoryPath);

		static void DisplayDirectoryContents(const string& directoryPath);
	};
}