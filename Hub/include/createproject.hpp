//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Graphics::GUI
{
	using std::string;

	class CreateProject
	{
	public:
		static inline bool renderCreateProjectWindow;

		static void RenderCreateProjectWindow();
		static void RenderCreateProjectContent();
	private:
		static inline string assignedProjectName = "Project";
		static constexpr int bufferSize = 16;

		static inline char projectName[bufferSize];

		static bool IsValidSymbolInPath(const char& c)
		{
			return c == '-'
				|| c == '_'
				|| c == ' '
				|| (c >= '0' && c <= '9')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z');
		}
	};
}