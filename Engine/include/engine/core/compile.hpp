//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <vector>
#include <string>

namespace Core
{
	using std::vector;
	using std::string;

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

		/// <summary>
		/// Main game compilation function, also calls RunInstaller.
		/// </summary>
		static void Compile();

		/// <summary>
		/// Compiles the game from source.
		/// </summary>
		static void RunInstaller();

		/// <summary>
		/// Draws the ImGui compilation window
		/// </summary>
		static void RenderBuildingWindow();

		/// <summary>
		/// Run the game exe.
		/// </summary>
		static void Run();

		/// <summary>
		/// Ensure the path to game exe and game parent are valid before using them to run game or compile it.
		/// </summary>
		static bool GamePathCheck();
	private:
		static inline bool finishedBuild;
		static inline bool firstScrollToBottom;
	};
}
#endif