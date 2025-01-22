//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <string>

namespace Graphics::GUI
{
	using std::string;

	class GUIImportAsset
	{
	public:
		static inline bool renderImportAsset;
		static inline string assetPath;

		static void RenderImportAsset();
	private:
		static inline string newName = "";
		static constexpr int bufferSize = 16;

		static inline char name[bufferSize];

		static void RenderImportAssetContent();
	};
}
#endif