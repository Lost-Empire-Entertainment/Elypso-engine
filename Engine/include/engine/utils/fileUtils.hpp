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
		/// Copy the selected file to the target path.
		/// </summary>
		/// <param name="filePath">Full path to the file you are trying to move.</param>
		/// <param name="targetPath">Full path to the target destination without the file name itself.</param>
		static void CopyFile(path& filePath, path& targetPath);
	};
}