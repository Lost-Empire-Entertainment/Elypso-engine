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

//engine
#include "fileUtils.hpp"
#include "searchUtils.hpp"
#include "console.hpp"

#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

using Utils::Search;
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

    void File::MoveOrRenameFileOrFolder(path& itemPath, path& targetPath, bool isRenaming)
    {
        string output;
        if (!isRenaming)
        {
            if (!PathExists(itemPath)
                || !PathExists(targetPath))
            {
                return;
            }
        }
        else
        {
            if (!PathExists(itemPath)) return;
            if (exists(targetPath))
            {
                output = "Error " + targetPath.string() + " already exists!\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::ENGINE,
                    Type::EXCEPTION,
                    output);

                return;
            }
        }

        try
        {
            rename(itemPath, targetPath);

            string outputType = isRenaming ? "Renamed " : "Moved ";
            output = outputType + itemPath.string() + " to " + targetPath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::SUCCESS,
                output);
        }
        catch (const exception& e)
        {
            output = "Error: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::CopyFileOrFolder(path& itemPath, path& targetPath)
    {
        string output;
        string fileOrFolderName = itemPath.filename().string();
        path fixedTargetPath = targetPath = targetPath.string() + "\\" + fileOrFolderName;
        if (!PathExists(itemPath))
        {
            return;
        }
        if (exists(targetPath))
        {
            output = "Error " + targetPath.string() + " already exists!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);

            return;
        }

        try
        {
            if (is_directory(itemPath))
            {
                create_directories(fixedTargetPath);

                //copy all subfiles and subfolders and item path to target path
                for (const auto& entry : recursive_directory_iterator(itemPath))
                {
                    path relativePath = relative(entry.path(), itemPath);
                    path destinationPath = fixedTargetPath / relativePath;

                    if (is_directory(entry))
                    {
                        create_directory(destinationPath);
                    }
                    else if (is_regular_file(entry))
                    {
                        copy_file(entry.path(), destinationPath, copy_options::overwrite_existing);
                    }
                }

                output = "Copied " + itemPath.string() + " and its sub-folders and sub-files (if there were any) to " + fixedTargetPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::ENGINE,
                    Type::SUCCESS,
                    output);
            }
            else
            {
                copy_file(itemPath, fixedTargetPath, copy_options::overwrite_existing);

                output = "Copied " + itemPath.string() + " to " + fixedTargetPath.string() + ".\n\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::ENGINE,
                    Type::SUCCESS,
                    output);
            }
        }
        catch (const exception& e)
        {
            output = "Error: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }

    void File::DeleteFileOrfolder(path& itemPath)
    {
        string output;
        if (!PathExists(itemPath)) return;

        try
        {
            if (is_regular_file(itemPath)) remove(itemPath);
            else if (is_directory(itemPath)) remove_all(itemPath);

            output = "Deleted " + itemPath.string() + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::SUCCESS,
                output);
        }
        catch (const exception& e)
        {
            output = "Error: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }

    bool File::PathExists(path& itemPath)
    {
        if (!exists(itemPath))
        {
            string output = "Error " + itemPath.string() + " does not exist!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }

        return (exists(itemPath));
    }

    void File::CreateNewFolder(path& folderPath)
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
                Type::SUCCESS,
                output);
        }
        catch (const exception& e)
        {
            output = "Error: " + string(e.what()) + ".\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);
        }
    }
}