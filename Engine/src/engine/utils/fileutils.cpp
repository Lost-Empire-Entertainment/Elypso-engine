//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#endif
#include <iostream>
#include <memory>
#include <string>

//engine
#include "fileutils.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringutils.hpp"
#include "gameobject.hpp"

using std::cout;
using std::exception;
using std::runtime_error;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::copy_file;
using std::filesystem::copy;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::copy_options;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::create_directory;
using std::filesystem::create_directories;
using std::filesystem::rename;
using std::filesystem::recursive_directory_iterator;
using std::filesystem::directory_iterator;
using std::to_string;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Graphics::Shape::GameObjectManager;

namespace Utils
{
    string File::GetOutputFromBatFile(const char* file)
    {
#ifdef _WIN32
        char buffer[128];
        string result = "";
        string command = "\"" + string(file) + "\"";
        FILE* pipe = _popen(command.c_str(), "r");

        if (!pipe) throw runtime_error("popen() failed!");

        try
        {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                result += buffer;
            }
        }
        catch (...)
        {
            _pclose(pipe);
            throw;
        }

        _pclose(pipe);

        return result;
#elif __linux__
        char buffer[128];
        string result = "";
        string command = "\"" + string(file) + "\"";
        FILE* pipe = popen(command.c_str(), "r");

        if (!pipe) throw runtime_error("popen() failed!");

        try
        {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                result += buffer;
            }
        }
        catch (...)
        {
            pclose(pipe);
            throw;
        }

        pclose(pipe);

        return result;
#endif
    }

    int File::RunBatFile(const string& file, bool runSeparate)
    {
        string command = runSeparate == true
            ? "start cmd /c \"" + file + "\""
            : "\"" + file + "\"";

        return system(command.c_str());
    }

    void File::RunApplication(const string& parentPath, const string& exePath)
    {
#ifdef _WIN32
        wstring wParentFolderPath(parentPath.begin(), parentPath.end());
        wstring wExePath(exePath.begin(), exePath.end());
        wstring wCommands{};

        //initialize structures for process creation
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);

        // Create the new process
        if (!CreateProcessW
        (
            wExePath.c_str(),          //path to the executable
            const_cast<LPWSTR>(wCommands.c_str()), //command line arguments
            nullptr,                   //process handle not inheritable
            nullptr,                   //thread handle not inheritable
            FALSE,                     //handle inheritance
            0,                         //creation flags
            nullptr,                   //use parent's environment block
            wParentFolderPath.c_str(), //use parent's starting directory
            &si,                       //pointer to STARTUPINFO structure
            &pi                        //pointer to PROCESS_INFORMATION structure
        ))
        {
            //retrieve the error code and print a descriptive error message
            LPVOID lpMsgBuf = nullptr;
            DWORD dw = GetLastError();
            FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER
                | FORMAT_MESSAGE_FROM_SYSTEM
                | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&lpMsgBuf, 0, nullptr);
            std::wcout << L"Error: " << reinterpret_cast<LPCWSTR>(lpMsgBuf) << L"\n\n";
            LocalFree(lpMsgBuf);
        }

        //close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#elif __linux__
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Error during fork");
            return;
        }

        if (pid == 0)
        {
            //detach from parent session
            if (setsid() < 0)
            {
                perror("Error creating new session");
                exit(EXIT_FAILURE);
            }

            //change working dir to game exe folder
            if (chdir(parentPath.c_str()) != 0)
            {
                perror("Error changing directory");
                exit(EXIT_FAILURE);
            }

            //empty arguments
            char* const argv[] = { const_cast<char*>(exePath.c_str()), nullptr };

            //replace child with executable
            execv(exePath.c_str(), argv);

            perror("Error launching exe process");
            exit(EXIT_FAILURE);
        }
#endif
    }

    void File::MoveOrRenameTarget(const string& originPath, const string& targetPath)
    {
        string output;

        if (!exists(originPath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Source path " + originPath + " does not exist!\n\n");
            return;
        }
        if (exists(path(targetPath)))
        {
            string outputType = path(originPath).parent_path() == path(targetPath).parent_path() ? "Cannot rename source '" : "Cannot move source '";
            output = outputType + originPath + "' to destination '" + targetPath + "' because destination already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
            return;
        }

        try
        {
            rename(originPath, targetPath);

            string outputType = path(originPath).parent_path() == path(targetPath).parent_path() ? "Renamed '" : "Moved '";
            output = outputType + originPath + "' to '" + targetPath + "'.\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = "Exception in File::MoveOrRenameTarget: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CopyTarget(const string& originPath, const string& targetPath)
    {
        string output;

        if (!exists(path(originPath)))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Source path '" + originPath + "' does not exist!\n\n");
            return;
        }

        try
        {
            if (is_directory(path(originPath)))
            {
                copy(path(originPath), path(targetPath), copy_options::recursive | copy_options::overwrite_existing);

                output = "Copied folder '" + originPath + "' to '" + targetPath + "'.\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::DEBUG,
                    output);
            }
            else if (is_regular_file(path(originPath)))
            {
                copy_file(path(originPath), path(targetPath), copy_options::overwrite_existing);

                output = "Copied file '" + originPath + "' to '" + targetPath + "'.\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::DEBUG,
                    output);
            }
        }
        catch (const exception& e)
        {
            output = "Exception in File::CopyTarget: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::DeleteTarget(const string& originPath)
    {
        string output;
        if (!exists(path(originPath)))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot delete file or folder '" + originPath + "' because it does not exist!\n\n");
            return;
        }

        try
        {
            if (is_regular_file(path(originPath))) remove(path(originPath));
            else if (is_directory(path(originPath)))
            {
                remove_all(path(originPath));
            }

            output = "Deleted '" + originPath + "'.\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = "Exception in File::DeleteTarget: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CreateNewFolder(const string& folderPath)
    {
        string output;
        if (exists(path(folderPath)))
        {
            output = "Error: " + folderPath + " already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);

            return;
        }

        if (is_regular_file(path(folderPath)))
        {
            output = "Error: " + folderPath + " must be a folder!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);

            return;
        }

        try
        {
            create_directory(path(folderPath));

            output = "Created new folder at '" + folderPath + "'.\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = "Exception in File::CreateNewFolder: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    bool File::IsValidSymbolInPath(char c)
    {
        return c == '-'
            || c == '_'
            || c == ' '
            || (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z');
    }

    string File::AddIndex(
        const string& parentFolderPath,
        const string& fileName,
        const string& extension)
    {
        string newFilePath = (path(parentFolderPath) / (fileName + extension)).string();

        if (exists(newFilePath))
        {
            //extract the actual base name (before the last '(')
            string baseName = fileName;
            size_t openParentPos = fileName.find_last_of('(');
            if (openParentPos != string::npos)
            {
                baseName = fileName.substr(0, openParentPos - 1);
            }

            //find highest available index for this exact base name
            int highestNumber = 1;
            for (const auto& entry : directory_iterator(parentFolderPath))
            {
                string entryName = path(entry).stem().string();

                //extract the base name of the existing entry
                string entryBaseName = entryName;
                size_t entryOpenParentPos = entryName.find_last_of('(');
                if (entryOpenParentPos != string::npos)
                {
                    entryBaseName = entryName.substr(0, entryOpenParentPos - 1);
                }

                //only compare entries with the exact same base name
                if (entryBaseName == baseName)
                {
                    string entryValue = GetValueBetweenParentheses(entryName);
                    if (!entryValue.empty() && all_of(entryValue.begin(), entryValue.end(), ::isdigit))
                    {
                        int entryConvertedValue = stoi(entryValue);
                        if (entryConvertedValue >= highestNumber)
                        {
                            highestNumber = entryConvertedValue + 1;
                        }
                    }
                }
            }

            //remove existing index (if any) before appending new one
            string cleanedFileName = fileName;
            size_t cleanOpenParentPos = fileName.find_last_of('(');
            size_t cleanCloseParentPos = fileName.find_last_of(')');

            if (cleanOpenParentPos != string::npos && cleanCloseParentPos != string::npos && cleanCloseParentPos > cleanOpenParentPos)
            {
                string potentialNumber = fileName.substr(cleanOpenParentPos + 1, cleanCloseParentPos - cleanOpenParentPos - 1);
                if (all_of(potentialNumber.begin(), potentialNumber.end(), ::isdigit))
                {
                    cleanedFileName = fileName.substr(0, cleanOpenParentPos - 1);
                }
            }

            //ensure cleaned filename is not empty
            if (cleanedFileName.empty()) cleanedFileName = fileName;

            //create new indexed filename
            string newFileName = cleanedFileName + " (" + to_string(highestNumber) + ")" + extension;
            newFilePath = (path(parentFolderPath) / newFileName).string();
        }

        return newFilePath;
    }
    string File::GetValueBetweenParentheses(const string& input)
    {
        size_t start = input.find('(');
        size_t end = input.find(')', start);

        if (start != string::npos
            && end != string::npos
            && end > start)
        {
            return input.substr(start + 1, end - start - 1);
        }

        return "";
    }
}