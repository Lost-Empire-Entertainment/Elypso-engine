//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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