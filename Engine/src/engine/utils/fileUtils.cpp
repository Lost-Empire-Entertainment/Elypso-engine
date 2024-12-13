//Copyright(C) 2024 Lost Empire Entertainment
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
#include "fileUtils.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
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

    int File::RunBatFile(const string& file, bool runSeparate, BatType type)
    {
        string command = runSeparate == true
            ? "start cmd /c \"" + file + "\""
            : "\"" + file + "\"";

        if (type == BatType::compile)
        {
            string param1 = " build";

            command += param1;
        }

        return system(command.c_str());
    }

    void File::RunApplication(const string& parentFolderPath, const string& exePath, const string& commands)
    {
#ifdef _WIN32
        wstring wParentFolderPath(parentFolderPath.begin(), parentFolderPath.end());
        wstring wExePath(exePath.begin(), exePath.end());
        wstring wCommands(commands.begin(), commands.end());

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
        //change working directory to parentFolderPath
        if (chdir(parentFolderPath.c_str()) != 0)
        {
            perror("Error changing directory");
            return;
        }

        //parse the commands into arguments
        vector<string> args;
        size_t pos = 0, found;
        while ((found = commands.find(' ', pos)) != string::npos)
        {
            args.push_back(commands.substr(pos, found - pos));
            pos = found + 1;
        }
        args.push_back(commands.substr(pos));

        //prepare arguments for execvp
        vector<char*> execArgs;
        execArgs.push_back(const_cast<char*>(exePath.c_str()));
        for (auto& arg : args)
        {
            execArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        execArgs.push_back(nullptr);

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("Error during fork");
            return;
        }

        if (pid == 0)
        {
            //child process: execute the program
            execvp(execArgs[0], execArgs.data());
            perror("Error during execvp");
            exit(EXIT_FAILURE); //exit if execvp fails
        }
        else
        {
            //parent process: wait for the child to finish
            int status;
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("Error during waitpid");
            }
            else if (WIFEXITED(status))
            {
                cout << "Child exited with status: " << WEXITSTATUS(status) << "\n";
            }
            else
            {
                cout << "Child did not exit normally\n";
            }
        }
#endif
    }

    void File::MoveOrRenameFileOrFolder(const path& sourcePath, const path& destinationPath, const bool isRenaming)
    {
        string output;

        if (!exists(sourcePath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Source path " + sourcePath.string() + " does not exist!\n\n");
            return;
        }
        if (exists(destinationPath))
        {
            string outputType = isRenaming ? "Cannot rename source " : "Cannot move source ";
            output = outputType + sourcePath.string() + " to destination " + destinationPath.string() + " because destination already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
            return;
        }

        try
        {
            rename(sourcePath, destinationPath);

            string outputType = isRenaming ? "Renamed " : "Moved ";
            output = outputType + sourcePath.string() + " to " + destinationPath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = "Exception in File::MoveOrRenameFileOrFolder: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CopyFileOrFolder(const path& sourcePath, const path& destinationPath)
    {
        string output;

        if (!exists(sourcePath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Source path " + sourcePath.string() + " does not exist!\n\n");
            return;
        }

        try
        {
            if (is_directory(sourcePath))
            {
                copy(sourcePath, destinationPath, copy_options::recursive | copy_options::overwrite_existing);

                output = "Copied folder " + sourcePath.string() + " to " + destinationPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::DEBUG,
                    output);
            }
            else if (is_regular_file(sourcePath))
            {
                copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);

                output = "Copied file " + sourcePath.string() + " to " + destinationPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::DEBUG,
                    output);
            }
        }
        catch (const exception& e)
        {
            output = "Exception in File::CopyFileOrFolder: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::DeleteFileOrfolder(const path& sourcePath)
    {
        string output;
        if (!exists(sourcePath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot delete file or folder " + sourcePath.string() + " because it does not exist!\n\n");
            return;
        }

        try
        {
            if (is_regular_file(sourcePath)) remove(sourcePath);
            else if (is_directory(sourcePath))
            {
                remove_all(sourcePath);
            }

            output = "Deleted " + sourcePath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = "Exception in File::DeleteFileOrFolder: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CreateNewFolder(const path& folderPath)
    {
        string output;
        if (exists(folderPath))
        {
            output = "Error: " + folderPath.string() + " already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);

            return;
        }

        if (is_regular_file(folderPath))
        {
            output = "Error: " + folderPath.string() + " must be a folder!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                output);

            return;
        }

        try
        {
            create_directory(folderPath);

            output = "Created new folder at " + folderPath.string() + ".\n\n";
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

    string File::AddIndex(
        const path& parentFolderPath, 
        const string& fileName, 
        const string& extension,
        const bool& bypassParenthesesCheck)
    {
        string newFilePath = (path(parentFolderPath) / (fileName + extension)).string();

        if (exists(newFilePath))
        {
            //simply add (1) after file/folder if it doesnt already have parentheses
            if (!bypassParenthesesCheck
                && fileName.find('(') == string::npos 
                && fileName.find(')') == string::npos)
            {
                string newName = fileName + " (1)" + extension;
                newFilePath = (path(parentFolderPath.string()) / newName).string();

                return newFilePath;
            }
            //try to create new file/folder with first highest available number
            else
            {
                string value = bypassParenthesesCheck ? "1" : GetValueBetweenParentheses(fileName);
                if (value == "")
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: Value between parentheses for '" + fileName + "' was empty so an index couldn't be added!\n");

                    return newFilePath;
                }

                int convertedValue = -1;
                try
                {
                    convertedValue = stoi(value);
                }
                catch (const exception& e) 
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: Exception occurred while processing value '" + value + "' for '" + fileName + "': " + e.what() + "\n");
                    
                    return newFilePath;
                }
                catch (...) 
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: An unknown exception occurred while processing value '" + value + "' for '" + fileName + "'\n");
                    
                    return newFilePath;
                }

                //return highest number compared to all existing gameobject folders
                int highestNumber = 1;
                for (const auto& entry : directory_iterator(parentFolderPath))
                {
                    string entryName = path(entry).stem().string();

                    if (is_directory(entry)
                        && GetValueBetweenParentheses(entryName) != "")
                    {
                        string entryValue = GetValueBetweenParentheses(entryName);
                        int entryConvertedValue = -1;

                        try
                        {
                            entryConvertedValue = stoi(entryValue);
                        }
                        catch (...)
                        {
                            ConsoleManager::WriteConsoleMessage(
                                Caller::FILE,
                                Type::DEBUG,
                                "Value between parentheses '" + entryValue + "' for '" + entryName + "' was not an integer so it was ignored.\n");
                            continue;
                        }

                        if (entryConvertedValue != -1
                            && entryConvertedValue >= highestNumber)
                        {
                            highestNumber = entryConvertedValue + 1;
                        }
                    }
                }

                string cleanedFileName = fileName;
                size_t pos = fileName.find('(');
                if (pos != string::npos)
                {
                    cleanedFileName = fileName.substr(0, pos);
                }

                cleanedFileName = cleanedFileName.empty() ? fileName : cleanedFileName;
                string newFileName = cleanedFileName + " (" + to_string(highestNumber) + ")" + extension;
                newFilePath = (path(parentFolderPath) / newFileName).string();
            }
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