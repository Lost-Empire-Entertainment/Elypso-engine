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
#include "admin.hpp"

namespace Core
{
    void Admin::RunAsAdmin()
    {
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        {
            std::cerr << "[ENGINE_EXCEPTION] Error opening process token.\n\n";
            ExitProcess(0);
            return;
        }

        BOOL isMember;
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID administratorsGroup;
        if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup))
        {
            std::cerr << "[ENGINE_EXCEPTION] Error initializing administrators group SID.";
            CloseHandle(token);
            ExitProcess(0);
            return;
        }

        if (!CheckTokenMembership(nullptr, administratorsGroup, &isMember))
        {
            std::cerr << "[ENGINE_EXCEPTION] Error checking token membership.\n\n";
            FreeSid(administratorsGroup);
            CloseHandle(token);
            ExitProcess(0);
            return;
        }

        FreeSid(administratorsGroup);
        CloseHandle(token);

        //if not running as admin, relaunch with elevated privileges
        if (!isMember)
        {
            wchar_t moduleName[MAX_PATH];
            GetModuleFileNameW(nullptr, moduleName, MAX_PATH);

            SHELLEXECUTEINFOW shellExecuteInfo = { sizeof(SHELLEXECUTEINFOW) };
            shellExecuteInfo.lpVerb = L"runas";
            shellExecuteInfo.lpFile = moduleName;
            shellExecuteInfo.nShow = SW_NORMAL;

            if (!ShellExecuteExW(&shellExecuteInfo))
            {
                DWORD error = GetLastError();
                if (error == ERROR_CANCELLED)
                {
                    std::wcerr << L"[ENGINE_EXCEPTION] User denied admin privileges.\n\n";
                    ExitProcess(0);
                }
                else
                {
                    std::wcerr << L"[ENGINE_EXCEPTION] Failed to obtain admin privileges. Error code: " << error + "\n\n";
                    ExitProcess(0);
                }
                return;
            }
        }
    }
}