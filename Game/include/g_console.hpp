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
		/// <summary>
		/// Add all game commands to the console manager
		/// </summary>
		static void AddCommands();
	private:
		/// <summary>
		/// Toggle game debug window on and off
		/// </summary>
		static void RenderDebugWindow(const vector<string>& args);
		/// <summary>
		/// Reset player position to start of level
		/// </summary>
		static void ResetPlayer(const vector<string>& args);
	};
}