//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
		static inline bool renderProjectHierarchy;
		static void RenderProjectHierarchy(const path& rootPath);

	private:
		static inline bool openRightClickPopup;
		static inline path selectedItemPath;

		/// <summary>
		/// Draw each individual folders content when it is accessed.
		/// </summary>
		/// <param name="folderPath">The folder path that is accessed.</param>
		static void DrawFolder(const path& folderPath, bool isRoot = false);

		/// <summary>
		/// Assigns the correct cleaned path to selectedItemPath when right-clicking a file or a folder in the project hierarchy and allows to render the popup.
		/// </summary>
		/// <param name="selectedItemPath">The path to the selected item.</param>
		static void RightClickConfirm(const path& selectedItemPath);

		/// <summary>
		/// Renders the actual popup itself.
		/// </summary>
		static void RenderPopup();
	};
}