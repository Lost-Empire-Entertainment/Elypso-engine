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

int main()
{
    string unconvertedExePath = "C:\\Path\\To\\YourExecutable.exe";
    string unconvertedIconPath = "C:\\Path\\To\\YourNewIcon.ico";

    if (!exists(unconvertedExePath))
    {
        cout << "Error: " << unconvertedExePath << " is not a valid path for the exe!" << endl;
        cin.get();
        return 1;
    }

    if (!exists(unconvertedIconPath))
    {
        cout << "Error: " << unconvertedIconPath << " is not a valid path for the icon!" << endl;
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
