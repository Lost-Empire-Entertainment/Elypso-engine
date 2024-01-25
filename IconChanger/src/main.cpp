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
#include <filesystem>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::path;

int main(int argc, char* argv[])
{
    const string inputexepath = argv[1];
    const string inputiconpath = argv[2];

    //load the icon
    HICON hIcon = (HICON)LoadImage(NULL, inputiconpath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

    //check if icon was loaded successfully
    if (!hIcon)
    {
        DWORD error = GetLastError();
        cout << "Error: Failed to load the icon! Error code: " << error << endl;
        cin.get();
        return 1;
    }

    //open the target executable file
    HANDLE hFile = BeginUpdateResource(inputexepath.c_str(), FALSE);

    //check if the file was opened successfully
    if (!hFile)
    {
        DWORD error = GetLastError();
        cout << "Error: Failed to open the executable file! Error code: " << error << endl;
        DestroyIcon(hIcon);
        return 1;
    }

    //try to find the icon resource
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(1), RT_ICON);

    //check if the resource was found
    if (!hResource)
    {
        DWORD error = GetLastError();
        cout << "Error: Failed to find the icon resource! Error code: " << error << endl;
        CloseHandle(hFile);
        DestroyIcon(hIcon);
        return 1;
    }

    //find the size of the icon resource
    DWORD iconSize = SizeofResource(NULL, hResource);

    //check if the resource size is valid
    if (iconSize == 0)
    {
        DWORD error = GetLastError();
        cout << "Error: Failed to obtain the size of the icon resource! Error code: " << error << endl;
        CloseHandle(hFile);
        DestroyIcon(hIcon);
        return 1;
    }

    //update the icon resource in the executable
    if (UpdateResource(hFile, RT_ICON, MAKEINTRESOURCE(1), 0, hIcon, iconSize))
    {
        //commit the changes
        EndUpdateResource(hFile, FALSE);
        cout << "Icon replaced successfully" << endl;
    }
    else
    {
        DWORD error = GetLastError();
        cout << "Error: Failed to replace icon! Error code: " << error << endl;
    }

    //close the file handle
    CloseHandle(hFile);

    //release the icon handle
    DestroyIcon(hIcon);

    cin.get();
    return 0;
}