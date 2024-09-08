//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <iostream>

#include "fileUtils.hpp"

using std::cout;
using std::exception;
using std::filesystem::create_directory;
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::directory_iterator;
using std::filesystem::remove_all;
using std::filesystem::remove;
using std::filesystem::path;
using std::wstring;

namespace Utils
{
	void File::CreateNewFolder(const string& folderPath)
	{
		string output;
		if (exists(folderPath))
		{
			cout << folderPath << " already exists!\n\n";

			return;
		}

		if (is_regular_file(folderPath))
		{
			cout << folderPath << " must be a folder!\n\n";

			return;
		}

		try
		{
			create_directory(folderPath);

			cout << "Created new folder at " << folderPath << ".\n\n";
		}
		catch (const exception& e)
		{
			cout << "Caught exception when creating new folder: " << string(e.what()) << ".\n\n";
		}
	}

	void File::DeleteFileOrfolder(const string& sourcePath)
	{
		string output;
		if (!exists(sourcePath))
		{
			cout << "Cannot delete file or folder " << sourcePath << " because it does not exist!\n\n";
			return;
		}

		try
		{
			if (is_regular_file(sourcePath)) remove(sourcePath);
			else if (is_directory(sourcePath))
			{
				bool hasChildDirectories = false;
				for (const auto& entry : directory_iterator(sourcePath))
				{
					if (is_directory(entry))
					{
						hasChildDirectories = true;
					}
					else if (is_regular_file(entry))
					{
						remove(entry);
					}
				}

				if (hasChildDirectories)
				{
					for (const auto& entry : directory_iterator(sourcePath))
					{
						DeleteFileOrfolder(path(entry).string());
					}
					remove_all(sourcePath);
				}
				else remove_all(sourcePath);
			}

			cout << "Deleted " << sourcePath << ".\n\n";
		}
		catch (const exception& e)
		{
			cout << "Caught exception when deleting file or folder: " << string(e.what()) << ".\n\n";
		}
	}

	string File::SetPath()
	{
		//initialize COM
		HRESULT hr = CoInitializeEx(
			NULL,
			COINIT_APARTMENTTHREADED
			| COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
		{
			cout << "File Explorer error: Failed to initialize COM!\n";
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
			cout << "File Explorer error: Failed to create File Open dialog!\n";
			CoUninitialize();
			return "";
		}

		//restrict the selection to folders only
		DWORD dwOptions;
		hr = pFileOpen->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
			if (FAILED(hr))
			{
				cout << "File Explorer error: Failed to set options!\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}
		else
		{
			cout << "File Explorer error: Failed to get options!\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//show the File Open dialog
		hr = pFileOpen->Show(NULL);
		if (FAILED(hr))
		{
			cout << "File Explorer error: Failed to show dialog!\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the result of the user's selection
		IShellItem* pItem;
		hr = pFileOpen->GetResult(&pItem);
		if (FAILED(hr))
		{
			cout << "File Explorer error: Failed to retrieve result!\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the path pf the selected file or folder
		PWSTR filePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
		if (FAILED(hr))
		{
			cout << "File Explorer error: Failed to retrieve path!\n";
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