//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Graphics::GUI
{
	using std::string;

	class RemoveProject
	{
	public:
		static inline bool renderRemoveProjectWindow;
		static inline string confirmFileName;
		static inline string confirmFilePath;

		static void RenderRemoveProjectWindow();
		static void RenderRemoveProjectContent();
	};
}