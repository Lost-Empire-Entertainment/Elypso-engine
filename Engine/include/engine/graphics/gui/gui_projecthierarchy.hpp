//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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
		static inline bool rightMouseClicked;
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