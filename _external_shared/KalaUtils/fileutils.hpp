//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAUTILS_DLL_EXPORT
		#define KALAUTILS_API __declspec(dllexport)
	#else
		#define KALAUTILS_API __declspec(dllimport)
	#endif
#else
	#define KALAUTILS_API
#endif

#include <string>
#include <filesystem>

namespace KalaKit
{
	using std::filesystem::path;
	using std::string;

	class KALAUTILS_API FileUtils
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
		/// <param name="commands">Additional optional commands.</param>
		static void RunApplication(const string& exePath, const string& commands = "");

		/// <summary>
		/// Check if the selected file contains the selected string.
		/// </summary>
		/// <param name="filePath">Where is the file located?</param>
		/// <param name="targetString">What is the string you are looking for?</param>
		static bool ContainsString(const string& filePath, const string& targetString);

		/// <summary>
		/// Move or rename the selected file or folder to the target path.
		/// It always renames if origin and target are in the same folder.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder that will be moved or renamed.</param>
		/// <param name="targetPath">Full path to where the file or folder will be moved to or renamed to.</param>
		static void MoveOrRenameTarget(const string& originPath, const string& targetPath);

		/// <summary>
		/// Rename the selected file or folder to the target path.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder that will be renamed.</param>
		/// <param name="targetPath">Full path to what the file or folder will be renamed to.</param>
		static void RenameTarget(const string& originPath, const string& targetPath);

		/// <summary>
		/// Copy the selected file or folder to the target path.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder you are trying to copy.</param>
		/// <param name="targetPath">Full path to the target destination.</param>
		static void CopyTarget(const string& originPath, const string& targetPath);

		/// <summary>
		/// Delete the selected file or folder.
		/// </summary>
		/// <param name="originPath">Full path to the file or folder you are trying to delete.</param>
		static void DeleteTarget(const string& targetPath);

		/// <summary>
		/// Create a new folder at the target destination.
		/// </summary>
		/// <param name="folderPath">Full path to the new folder you are trying to create the folder to.</param>
		static void CreateNewFolder(const string& folderPath);

		/// <summary>
		/// Add an index (1), etc after this file or folder name.
		/// </summary>
		/// <param name="folderPath">Parent folder of this file</param>
		/// <param name="fileName">Name of the file the index will be added after</param>
		/// <param name="extension">Name of the extension if this is a file</param>
		static string AddIndex(
			const path& folderPath,
			const string& fileName,
			const string& extension = "");
	private:
		static string GetValueBetweenParentheses(const string& input);
	};
}