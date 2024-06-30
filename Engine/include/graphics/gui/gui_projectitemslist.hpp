//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUIProjectItemsList
	{
	public:
		static inline enum class Type
		{
			Gameobjects,
			Textures,
			Scenes
		};

		static inline bool renderProjectItemsList;

		static void RenderProjectItemsList(const Type& type);
	private:
		static void RenderProjectItemsListContent(const Type& type);
	};
}