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

#include <string>
#include <filesystem>

using std::string;
using std::filesystem::path;

namespace Utils
{
	class File
	{
	public:
		/// <summary>
		/// Return the output from a bat file as a string.
		/// </summary>
		/// <param name="file">Where is the file located at?</param>
		/// <returns></returns>
		static string GetOutputFromBatFile(const char* file);

		/// <summary>
		/// Run a bat file.
		/// </summary>
		/// <param name="file"></param>
		static void RunBatFile(const char* file);

		/// <summary>
		/// Move or rename the selected file or folder.
		/// </summary>
		/// <param name="sourcePath">Full path to the file or folder you are trying to move or rename.</param>
		/// <param name="destinationPath">Full path to the target destination.</param>
		/// <param name="isRenaming">Should the file or folder be renamed?</param>
		static void MoveOrRenameFileOrFolder(const path& sourcePath, const path& destinationPath, const bool isRenaming);

		/// <summary>
		/// Copy the selected file or folder to the target path.
		/// </summary>
		/// <param name="sourcePath">Full path to the file or folder you are trying to copy.</param>
		/// <param name="destinationPath">Full path to the target destination.</param>
		static void CopyFileOrFolder(const path& sourcePath, const path& destinationPath);

		/// <summary>
		/// Delete a file or folder.
		/// </summary>
		/// <param name="sourcePath">Full path to the file or folder you are trying to delete.</param>
		static void DeleteFileOrfolder(const path& sourcePath);

		/// <summary>
		/// Create a new folder at the target destination.
		/// </summary>
		/// <param name="folderPath">Full path to the new folder you are trying to create the folder to.</param>
		static void CreateNewFolder(const path& folderPath);
	};
}