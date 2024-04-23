//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>

using std::string;

namespace Graphics::GUI
{
	class GUIImportAsset
	{
	public:
		static inline bool renderImportAsset;
		static inline string targetModelPath;

		static void RenderImportAsset();
	private:

	};
}