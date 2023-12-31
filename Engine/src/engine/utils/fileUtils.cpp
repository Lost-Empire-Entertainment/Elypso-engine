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
using std::runtime_error;
using std::filesystem::exists;
using std::filesystem::copy_file;
using std::filesystem::copy_options;

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

    void File::CopyFile(path& filePath, path& targetDestination)
    {
        string output;
        if (!exists(filePath))
        {
            output = "Error " + filePath.string() + " does not exist!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);

            return;
        }

        if (!exists(targetDestination))
        {
            output = "Error " + targetDestination.string() + " does not exist!\n\n";
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                output);

            return;
        }

        string fileName = filePath.filename().string();
        string targetDestinationWithFile = targetDestination.string() + "\\" + fileName;
        copy_file(filePath, targetDestinationWithFile, copy_options::overwrite_existing);

        output = "Moved " + filePath.string() + " to " + targetDestination.string() + "\n\n";
        ConsoleManager::WriteConsoleMessage(
            Caller::ENGINE,
            Type::INFO,
            output);
    }
}