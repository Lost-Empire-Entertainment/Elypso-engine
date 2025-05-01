//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

namespace GameTemplate
{
	using std::string;
	using std::vector;

	class G_Console
	{
	public:
		static void AddCommands();
	private:
		static void RenderDebugWindow(const vector<string>& args);
		static void CreateCube(const vector<string>& args);
	};
}