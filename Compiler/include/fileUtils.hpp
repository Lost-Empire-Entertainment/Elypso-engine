//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Utils
{
	class File
	{
	public:
		static void CreateNewFolder(const string& folderPath);
		static void DeleteFileOrfolder(const string& sourcePath);
		static string SetPath();
	};
}