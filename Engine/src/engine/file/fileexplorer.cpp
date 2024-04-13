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

#include <Windows.h>
#include <ShlObj.h>
#include <iostream>

//engine
#include "fileexplorer.hpp"

using std::cout;
using std::wstring;

namespace EngineFile
{
	string FileExplorer::Select(const SearchType& searchType)
	{
		//initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
		{
			cout << "Error: Failed to initialize COM!\n\n";
			return "";
		}

		//create an instance of the File Open dialog
		IFileOpenDialog* pFileOpen = nullptr;
		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			NULL,
			CLSCTX_ALL,
			IID_IFileOpenDialog,
			reinterpret_cast<void**>(&pFileOpen));
		if (FAILED(hr))
		{
			cout << "Error: Failed to create File Open dialog!\n\n";
			CoUninitialize();
			return "";
		}

		if (searchType == SearchType::txt)
		{
			//restrict file selection to txt only
			COMDLG_FILTERSPEC filterSpec[] = { { L"Level files", L"*.txt"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set file filter!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::asset)
		{
			//restrict file selection to fbx, obj and gltf only
			COMDLG_FILTERSPEC filterSpec[] = {
				{ L"fbx objects", L"*.fbx"},
				{ L"obj objects", L"*.obj"},
				{ L"glTF objects", L"*.gltf"} };
			hr = pFileOpen->SetFileTypes(3, filterSpec);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set file filter!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::texture)
		{
			//restrict file selection to png, jpg and jpeg only
			COMDLG_FILTERSPEC filterSpec[] = {
				{ L"png textures", L"*.png"},
				{ L"jpg textures", L"*.jpg"},
				{ L"jpeg textures", L"*.jpeg"} };
			hr = pFileOpen->SetFileTypes(3, filterSpec);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set file filter!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		//show the File Open dialog
		hr = pFileOpen->Show(NULL);
		if (FAILED(hr))
		{
			cout << "Error: Failed to show dialog!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the result of the user's selection
		IShellItem* pItem;
		hr = pFileOpen->GetResult(&pItem);
		if (FAILED(hr))
		{
			cout << "Error: Failed to retrieve result!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the path pf the selected file or folder
		PWSTR filePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
		if (FAILED(hr))
		{
			cout << "Error: Failed to retrieve path!\n\n";
			pItem->Release();
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//convert the wide string to a string
		wstring ws(filePath);

		//get the required buffer size
		int size_needed = WideCharToMultiByte(
			CP_UTF8,
			0,
			ws.c_str(),
			static_cast<int>(ws.length()),
			NULL,
			0,
			NULL,
			NULL);

		//convert wide string to utf-8 encoded narrow string
		string narrowPath(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			ws.c_str(),
			static_cast<int>(ws.length()),
			&narrowPath[0],
			size_needed,
			NULL,
			NULL);

		//free memory allocated for filePath
		CoTaskMemFree(filePath);

		//release the shell item
		pItem->Release();

		//release the file open dialog
		pFileOpen->Release();

		//uninitialze COM
		CoUninitialize();

		return narrowPath;
	}
}