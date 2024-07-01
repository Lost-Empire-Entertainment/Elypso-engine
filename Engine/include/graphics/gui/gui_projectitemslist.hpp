//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Graphics::GUI
{
	class GUIProjectItemsList
	{
	public:
		enum class Type
		{
			Textures,
			Scenes
		};

		static inline bool renderProjectItemsList;
		static inline Type type;

		static void RenderProjectItemsList();
	private:
		static inline bool isContentVectorFilled;
		static inline string selectedPath;
		static inline vector<string> content;

		static void RenderProjectItemsListContent();
	};
}