//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#include <vector>
#include <string>

namespace Graphics::GUI
{
	using std::vector;
	using std::string;

	class GUIConsole
	{
	public:
#if ENGINE_MODE
#else
		static inline bool renderConsole;
#endif
		static inline bool firstScrollToBottom;

		static inline vector<string> consoleMessages;

		static void RenderConsole();
		static void AddTextToConsole(const string& message);
	private:
		static inline char inputTextBuffer[128];
		static constexpr int maxConsoleMessages = 1000;
	};
}