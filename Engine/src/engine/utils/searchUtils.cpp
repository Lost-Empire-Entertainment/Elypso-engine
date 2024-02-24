//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>

//engine
#include "searchUtils.hpp"
#include "stringUtils.hpp"

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

			//get the required buffer size
			int size_needed = WideCharToMultiByte(
				CP_UTF8, 
				0, 
				wPath.c_str(),
				static_cast<int>(wPath.length()), 
				NULL, 
				0, 
				NULL, 
				NULL);

			//convert wide string to utf-8 encoded narrow string
			string narrowPath(size_needed, 0);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				&narrowPath[0],
				size_needed,
				NULL,
				NULL);

			return String::StringReplace(
				string(narrowPath.begin(), narrowPath.end()), "\\", "/") +
				"/" + "Elypso engine";
		}
		else return "";
	}
}