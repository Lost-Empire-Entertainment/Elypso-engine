//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <iostream>

//engine
#include "fileexplorer.hpp"
#include "console.hpp"

using std::cout;
using std::wstring;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	string FileExplorer::Select(const SearchType& searchType)
	{
		//initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"File Explorer error: Failed to initialize COM!\n");
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
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"File Explorer error: Failed to create File Open dialog!\n");
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
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to set file filter!\n");
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::asset)
		{
			//restrict file selection to any extension
			COMDLG_FILTERSPEC filterSpec[] = {
				{ L"Assets", L"*.*"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to set file filter!\n");
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
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to set file filter!\n");
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::exe)
		{
			//restrict file selection to exe only
			COMDLG_FILTERSPEC filterSpec[] = {
				{ L"Executables", L"*.exe"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to set file filter!\n");
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::bat)
		{
			//restrict file selection to bat only
			COMDLG_FILTERSPEC filterSpec[] = {
				{ L"Batch files", L"*.bat"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to set file filter!\n");
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (searchType == SearchType::folder)
		{
			//restrict the selection to folders only
			DWORD dwOptions;
			hr = pFileOpen->GetOptions(&dwOptions);
			if (SUCCEEDED(hr))
			{
				hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
				if (FAILED(hr))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"File Explorer error: Failed to set options!\n");
					pFileOpen->Release();
					CoUninitialize();
					return "";
				}
			}
			else
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"File Explorer error: Failed to get options!\n");
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		//show the File Open dialog
		hr = pFileOpen->Show(NULL);
		if (FAILED(hr))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"File Explorer error: Failed to show dialog!\n");
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the result of the user's selection
		IShellItem* pItem;
		hr = pFileOpen->GetResult(&pItem);
		if (FAILED(hr))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"File Explorer error: Failed to retrieve result!\n");
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the path pf the selected file or folder
		PWSTR filePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
		if (FAILED(hr))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"File Explorer error: Failed to retrieve path!\n");
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