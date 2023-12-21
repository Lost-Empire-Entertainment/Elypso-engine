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

//engine
#include "searchUtils.hpp"
#include "stringUtils.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>

using std::wstring;
using std::filesystem::path;
using std::filesystem::current_path;

namespace Utils
{
	string Search::FindCurrentPath()
	{
		return current_path().string();
	}

	string Search::FindDocumentsFolder()
	{
		PWSTR path;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
		if (SUCCEEDED(result))
		{
			wstring wPath(path);
			CoTaskMemFree(path); //free the allocated memory

			return String::Replace(
				string(wPath.begin(), wPath.end()), "\\", "/") + 
				"/" + "Elypso engine";
		}
		else return "";
	}
}