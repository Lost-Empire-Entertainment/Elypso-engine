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
		static inline bool checkBoxMapFilled;
		static inline map<string, bool> checkboxStates;

		static inline const int nameBufferSize = 32;
		static inline const int pathBufferSize = 256;

		static inline string assignedName = "Model";
		static inline string assignedDiffuseTexture;

		static inline char inputTextBuffer_objName[nameBufferSize];
		static inline char inputTextBuffer_diffTexturePath[pathBufferSize];
	};
}