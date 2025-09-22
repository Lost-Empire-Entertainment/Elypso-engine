//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace EngineFile
{
	using std::string;

	class FileExplorer
	{
	public:
		enum class SearchType
		{
			txt,
			asset,
			texture,
			exe,
			bat,
			folder
		};

		static string Select(const SearchType& searchType);
	};
}