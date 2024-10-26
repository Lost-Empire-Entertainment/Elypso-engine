//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Core
{
	class TheCompiler
	{
	public:
		static inline bool isCompiling;
		static inline bool finishedEngineBuild;
		static inline bool finishedLibraryBuild;

		enum class CompileType
		{
			compile,
			clean_rebuild
		};
		static inline CompileType compileType;

		static void Compile();
	};
}