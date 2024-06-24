//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>
#include <map>

using std::string;
using std::map;

namespace Graphics::GUI
{
	class GUIImportAsset
	{
	public:
		static inline bool renderImportAsset;
		static inline string assetPath;

		static void RenderImportAsset();
	private:
		static inline string newName = "Name";
		static constexpr int bufferSize = 16;

		static inline char name[bufferSize];

		static void RenderImportAssetContent();
	};
}