//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>

namespace Utils
{
	using std::string;
	using std::filesystem::path;

	class File
	{
	public:
		/// <summary>
		/// Return the output from a bat file as a string.
		/// </summary>
		/// <param name="file">Where is the file located at?</param>
		/// <returns></returns>
		static string GetOutputFromBatFile(const char* file);

		enum class BatType
		{
			none,
			compile
		};
		/// <summary>
		/// Run a bat file.
		/// </summary>
		/// <param name="file"></param>
		/// <param name="run separate"></param>
		/// <param name="type"></param>
		/// <returns></returns>
		static int RunBatFile(const string& file, bool runSeparate, BatType type);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="parentFolderPath">Where the exe is ran from.</param>
		/// <param name="exePath">Direct path to exe itself.</param>
		/// <param name="commands">Additional optional commands.</param>
		static void RunApplication(const string& parentFolderPath, const string& exePath, const string& commands = "");

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

		/// <summary>
		/// Add an index (1), etc after this file or folder name.
		/// </summary>
		/// <param name="folderPath">Parent folder of this file</param>
		/// <param name="fileName">Name of the file the index will be added after</param>
		/// <param name="bypassParenthesesCheck">We need this bool to skip adding the default (1) to all template gameobjects because 'Import Asset' always uses default placeholder name with no index</param>
		static string AddIndex(
			const path& folderPath, 
			const string& fileName, 
			const string& extension = "",
			const bool& bypassParenthesesCheck = false);
	private:
		static string GetValueBetweenParentheses(const string& input);
	};
}