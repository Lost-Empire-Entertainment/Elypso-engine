//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <string>

namespace Graphics::GUI
{
	using std::string;

	class GUIRename
	{
	public:
		static inline string extension;
		static inline string originalName;
		static inline string parentFolder;

		static inline bool renderRenameWindow;

		static void RenderRenameWindow();
	private:
		static inline string newName = "Name";
		static constexpr int bufferSize = 16;

		static inline char name[bufferSize];

		static void RenderRenameContent();
	};
}
#endif