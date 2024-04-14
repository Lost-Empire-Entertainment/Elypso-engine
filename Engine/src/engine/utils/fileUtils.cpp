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

#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

//engine
#include "fileUtils.hpp"
#include "console.hpp"

using std::exception;
using std::runtime_error;
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

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Utils
{
	string File::GetOutputFromBatFile(const char* file)
	{
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
	}

    void File::RunBatFile(const char* file)
    {
        string command = "\"" + string(file) + "\"";
        FILE* pipe = _popen(command.c_str(), "r");

        if (!pipe) throw runtime_error("popen() failed!");

        _pclose(pipe);
    }

    void File::MoveOrRenameFileOrFolder(const path& sourcePath, const path& destinationPath, const bool isRenaming)
    {
        string output;

        if (!exists(sourcePath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                "Source path " + sourcePath.string() + " does not exist!\n\n");
            return;
        }
        if (exists(destinationPath))
        {
            string outputType = isRenaming ? "Cannot rename source " : "Cannot move source ";
            output = outputType + sourcePath.string() + " to destination " + destinationPath.string() + " because destination already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
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
                Caller::ENGINE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CopyFileOrFolder(const path& sourcePath, const path& destinationPath)
    {
        string output;
        string fileOrFolderName = sourcePath.filename().string();
        path fixedTargetPath = destinationPath / fileOrFolderName;

        if (!exists(sourcePath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                "Source path " + sourcePath.string() + " does not exist!\n\n");
            return;
        }

        try
        {
            if (is_directory(sourcePath))
            {
                create_directories(fixedTargetPath);

                for (const auto& entry : recursive_directory_iterator(sourcePath))
                {
                    path relativePath = relative(entry.path(), sourcePath);
                    path destPath = fixedTargetPath / relativePath;

                    if (is_directory(entry))
                    {
                        create_directory(destPath);
                    }
                    else if (is_regular_file(entry))
                    {
                        copy_file(entry.path(), destPath, copy_options::overwrite_existing);
                    }
                }

                output = "Copied folder " + sourcePath.string() + " to " + fixedTargetPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::ENGINE,
                    Type::DEBUG,
                    output);
            }
            else if (is_regular_file(sourcePath))
            {
                create_directories(destinationPath);

                path destPath = destinationPath / fileOrFolderName;
                copy_file(sourcePath, destPath, copy_options::overwrite_existing);

                output = "Copied file " + sourcePath.string() + " to " + destPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::ENGINE,
                    Type::DEBUG,
                    output);
            }
        }
        catch (const exception& e)
        {
            output = string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
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
                Caller::ENGINE,
                Type::EXCEPTION,
                sourcePath.string() + " does not exist!\n\n");
            return;
        }

        try
        {
            if (is_regular_file(sourcePath)) remove(sourcePath);
            else if (is_directory(sourcePath)) remove_all(sourcePath);

            output = "Deleted " + sourcePath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CreateNewFolder(const path& folderPath)
    {
        string output;
        if (exists(folderPath))
        {
            output = "Error " + folderPath.string() + " already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);

            return;
        }

        if (is_regular_file(folderPath))
        {
            output = "Error " + folderPath.string() + " must be a folder!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);

            return;
        }

        try
        {
            create_directory(folderPath);

            output = "Created new folder at " + folderPath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::DEBUG,
                output);
        }
        catch (const exception& e)
        {
            output = string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }
}