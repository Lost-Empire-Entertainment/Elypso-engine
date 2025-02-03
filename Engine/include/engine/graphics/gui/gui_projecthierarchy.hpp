//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <string>
#include <vector>

namespace Graphics::GUI
{
	using std::string;
	using std::vector;

	class GUIProjectHierarchy
	{
	public:
		static void RenderProjectHierarchy();
	private:
		static void DisplayDirectoryContents(const string& directoryPath);
	};
}
#endif