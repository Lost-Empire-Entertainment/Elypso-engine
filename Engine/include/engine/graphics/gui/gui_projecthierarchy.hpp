//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <vector>
#include <string>

using std::filesystem::path;
using std::vector;
using std::string;

namespace Graphics::GUI
{
	class GUIProjectHierarchy
	{
	public:
		static inline path rootPath;
		static inline bool renderProjectHierarchy;
		static void RenderProjectHierarchy(const path& rootPath);

	private:
		static inline char newNameBuffer[64];
		static inline bool isRenaming;
		static inline bool isFolder;
		static inline path copyPath;
		static inline path selectedItemPath;

		/// <summary>
		/// Draw each individual folders content when it is accessed.
		/// </summary>
		/// <param name="folderPath">The folder path that is accessed.</param>
		static void DrawFolder(const path& folderPath, bool isRoot);

		/// <summary>
		/// Renders the popup that allows to do different actions when right-clicking a file or a folder in the project hierarchy.
		/// </summary>
		static void RightClickPopup();

		static void RenameFileOrFolder();
	};
}