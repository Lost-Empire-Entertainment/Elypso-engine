//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Core
{
	class Compilation
	{
	public:
		enum class InstallerType
		{
			compile,
			reset
		};
		static inline InstallerType installerType;

		static inline bool renderBuildingWindow;

		static inline vector<string> output;

		static void Compile();

		static void RenderBuildingWindow();

		static void RunInstaller();
	private:
		static inline bool finishedBuild;
		static inline bool firstScrollToBottom;
	};
}