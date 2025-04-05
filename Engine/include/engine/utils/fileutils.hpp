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

		/// <summary>
		/// Run a bat file.
		/// </summary>
		/// <param name="file"></param>
		/// <param name="run separate"></param>
		static int RunBatFile(const string& file, bool runSeparate);

		/// <summary>
		/// Run an executable.
		/// </summary>
		/// <param name="exePath">Direct path to exe itself.</param>
		static void RunApplication(const string& parentPath, const string& exePath);

		/// <summary>
		/// Move or rename the selected file or folder.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder you are trying to move or rename.</param>
		/// <param name="targetPath">Full path to the target destination.</param>
		/// <param name="isRenaming">Should the file or folder be renamed?</param>
		static void MoveOrRenameTarget(const string& originPath, const string& targetPath);

		/// <summary>
		/// Copy the selected file or folder to the target path.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder you are trying to copy.</param>
		/// <param name="targetPath">Full path to the target destination.</param>
		static void CopyTarget(const string& originPath, const string& targetPath);

		/// <summary>
		/// Delete a file or folder.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder you are trying to delete.</param>
		static void DeleteTarget(const string& originPath);

		/// <summary>
		/// Create a new folder at the target destination.
		/// </summary>
		/// <param name="folderPath">Full path to the new folder you are trying to create the folder to.</param>
		static void CreateNewFolder(const string& folderPath);

		/// <summary>
		/// Check if the character is allowed in paths in Windows
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsValidSymbolInPath(char c);

		/// <summary>
		/// Add an index (1), etc after this file or folder name.
		/// </summary>
		/// <param name="folderPath">Parent folder of this file</param>
		/// <param name="fileName">Name of the file the index will be added after</param>
		static string AddIndex(
			const string& folderPath,
			const string& fileName,
			const string& extension = "");
	private:
		static string GetValueBetweenParentheses(const string& input);
	};
}