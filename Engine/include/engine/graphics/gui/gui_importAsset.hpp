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
		bool checkBoxMapFilled;

		static constexpr int nameBufferSize = 32;
		static constexpr int pathBufferSize = 256;

		string assignedName = "Model";
		string assignedDiffuseTexture;

		char inputTextBuffer_objName[nameBufferSize];
		char inputTextBuffer_diffTexturePath[pathBufferSize];
	};
}