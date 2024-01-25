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

#include <windows.h>
#include <filesystem>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::filesystem::current_path;
using std::filesystem::exists;

int main(int argc, char* argv[])
{
    if (argc == 0)
    {
        cout << "Error: No argument was passed to IconChanger.exe! This program should not be ran directly." << endl;
        cin.get();
        return 1;
    }

    const char* exepath = argv[1];
    const char* iconpath = argv[2];

    if (!exists(exepath))
    {
        cout << "Error: " << exepath << " is not a valid path for the exe!" << endl;
        cin.get();
        return 1;
    }

    if (!exists(iconpath))
    {
        cout << "Error: " << iconpath << " is not a valid path for the icon!" << endl;
        cin.get();
        return 1;
    }

    const wchar_t* exePath = L"C:\\Path\\To\\YourExecutable.exe";
    const wchar_t* iconPath = L"C:\\Path\\To\\YourNewIcon.ico";

    HANDLE hUpdate = BeginUpdateResourceW(exePath, FALSE);
    if (hUpdate == NULL)
    {
        // Handle error, could not open the executable
        cout << "Error: Could not open the executable!" << endl;
        cin.get();
        return 1;
    }

    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(1), RT_ICON);
    if (hResource == NULL)
    {
        EndUpdateResource(hUpdate, TRUE);
        cout << "Error: Could not find the resource!" << endl;
        cin.get();
        return 1;
    }

    HGLOBAL hGlobal = LoadResource(NULL, hResource);
    if (hGlobal == NULL)
    {
        EndUpdateResource(hUpdate, TRUE);
        cout << "Error: Could not load the resource!" << endl;
        cin.get();
        return 1;
    }

    void* pData = LockResource(hGlobal);
    DWORD dwSize = SizeofResource(NULL, hResource);

    if (!UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pData, dwSize))
    {
        EndUpdateResource(hUpdate, TRUE);
        cout << "Error: Could not update the resource!" << endl;
        cin.get();
        return 1;
    }

    if (!EndUpdateResource(hUpdate, FALSE))
    {
        cout << "Error: Could not save changes!" << endl;
        cin.get();
        return 1;
    }

    return 0;
}
