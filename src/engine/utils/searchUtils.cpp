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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//engine
#include "searchUtils.hpp"

#include <string>
#include <filesystem>
#include <iostream>

using std::string;
using std::filesystem::path;
using std::filesystem::current_path;

namespace Utils
{
	string Search::SearchByParent(const string& targetFolderName)
	{
		//get the current working directory
		path currentPath = current_path();
		path current = currentPath;

		//iterate until we reach the root directory
		while (!current.empty())
		{
			//check if the target folder exists in the current directory
			if (is_directory(current / targetFolderName))
			{
				return (current / targetFolderName).string();
			}

			current = current.parent_path();
		}

		//if no folder was found
		return "";
	}
}