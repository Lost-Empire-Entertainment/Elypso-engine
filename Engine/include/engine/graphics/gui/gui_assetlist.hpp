//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUIAssetList
	{
	public:
		static inline bool renderAssetList;

		static void RenderAssetList();
	private:
		static void DrawCategoriesHierarchy();
		static void DrawGameobjectsTable();
	};
}