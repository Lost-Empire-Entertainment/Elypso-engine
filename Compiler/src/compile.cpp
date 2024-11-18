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
using std::filesystem::path;

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
				isCompiling = true;

				string hubBuildFolder = (path(Compiler::projectsPath) / "Hub" / "build").string();
				string engineBuildFolder = (path(Compiler::projectsPath) / "Engine" / "build").string();
				string engineLibraryFolder = (path(Compiler::projectsPath) / "Engine library" / "build").string();

				string buildFolder;
				if (GUI::target == GUI::Target::Hub) buildFolder = hubBuildFolder;
				if (GUI::target == GUI::Target::Engine)
				{
					buildFolder = !finishedEngineBuild 
						? engineBuildFolder 
						: engineLibraryFolder;
				}

				string command = "";

				switch (compileType)
				{
				case CompileType::clean_rebuild:
				{
					if (exists(buildFolder))
					{
						File::DeleteFileOrfolder(buildFolder);
					}
					File::CreateNewFolder(buildFolder);

#ifdef _WIN32
					command =
						"cd " + buildFolder +
						" && cmake -A x64 .."
						" && cmake --build . --config Release -- /m";

					command = "cmd /c \"" + command + "\"";
#elif __linux__
					command =
						"cd " + buildFolder +
						" && cmake -S .. -B " + buildFolder + " -DCMAKE_BUILD_TYPE=Release"
						" && make -j";
#endif
					break;
				}
				case CompileType::compile:
				{
#ifdef _WIN32
					if (exists(buildFolder))
					{
						command =
							"cd " + buildFolder +
							+" && cmake --build . --config Release -- /m";
					}
					else
					{
						File::CreateNewFolder(buildFolder);

						command =
							"cd " + buildFolder +
							" && cmake -A x64 .."
							" && cmake --build . --config Release -- /m";
					}

					command = "cmd /c \"" + command + "\"";
#elif __linux__
					if (exists(buildFolder))
					{
						command =
							"cd " + buildFolder +
							" && make -j";
					}
					else
					{
						File::CreateNewFolder(buildFolder);

						command =
							"cd " + buildFolder +
							" && cmake -S .. -B " + buildFolder + " -DCMAKE_BUILD_TYPE=Release"
							" && make -j";
					}
#endif
					break;
				}
				}

				//command to run the batch file and capture errors
				string fullCommand = command + " 2>&1"; //redirect stderr to stdout

				array<char, 128> buffer{};
#ifdef _WIN32
				unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(fullCommand.c_str(), "r"), _pclose);
#elif __linux__
				auto pipe = unique_ptr<FILE, void(*)(FILE*)>(
					popen(fullCommand.c_str(), "r"),
					[](FILE* file) { if (file) pclose(file); }
				);
#endif
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

				if (!finishedEngineBuild
					&& !finishedLibraryBuild)
				{
					finishedEngineBuild = true;
				}
				else if (finishedEngineBuild
						 && !finishedLibraryBuild)
				{
					finishedLibraryBuild = true;
				}

				GUI::FinishCompile();
			});

		CompileThread.detach();
	}
}