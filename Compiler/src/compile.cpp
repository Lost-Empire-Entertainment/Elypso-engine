//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <memory>
#include <array>
#include <iostream>
#include <thread>

#include "compile.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "fileUtils.hpp"

using std::filesystem::exists;
using std::unique_ptr;
using std::array;
using std::cout;
using std::runtime_error;
using std::thread;

using Graphics::GUI;
using Core::Compiler;
using Graphics::GUI;
using Utils::File;

namespace Core
{
	void TheCompiler::Compile()
	{
		thread CompileThread([]()
			{
				string hubBuildFolder = Compiler::hubFolderPath + "\\build";
				string engineBuildFolder = Compiler::engineFolderPath + "\\build";
				string buildFolder = GUI::target == GUI::Target::Hub
					? hubBuildFolder
					: engineBuildFolder;

				string command = "";

				switch (compileType)
				{
				case CompileType::compile:
				{
					if (exists(buildFolder))
					{
						command =
							"cd " + buildFolder +
							+" && cmake --build . --config Release -- /m";
					}
					else
					{
						File::DeleteFileOrfolder(buildFolder);
						File::CreateNewFolder(buildFolder);

						command =
							"cd " + buildFolder +
							+" && cmake -A x64 .." +
							+" && cmake --build . --config Release -- /m";
					}

					command = "cmd /c \"" + command + "\"";
					break;
				}
				case CompileType::clean_rebuild:
				{
					if (!exists(buildFolder))
					{
						File::CreateNewFolder(buildFolder);
					}
					if (exists(buildFolder))
					{
						File::DeleteFileOrfolder(buildFolder);
						File::CreateNewFolder(buildFolder);
					}

					command =
						"cd " + buildFolder +
						+" && cmake -A x64 .." +
						+" && cmake --build . --config Release -- /m";

					command = "cmd /c \"" + command + "\"";
					break;
				}
				}

				//command to run the batch file and capture errors
				string fullCommand = command + " 2>&1"; //redirect stderr to stdout

				array<char, 128> buffer{};
				unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(fullCommand.c_str(), "r"), _pclose);

				if (!pipe)
				{
					throw runtime_error("_popen() failed!");
				}

				//read the output line by line and add to the provided vector
				while (fgets(
					buffer.data(),
					static_cast<int>(buffer.size()),
					pipe.get()) != nullptr)
				{
					GUI::output.emplace_back(buffer.data());
				}

				GUI::FinishCompile();
			});

		CompileThread.detach();
	}
}