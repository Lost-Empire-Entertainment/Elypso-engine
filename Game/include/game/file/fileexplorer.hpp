//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace GameFile
{
	class FileExplorer
	{
	public:
		enum class SearchType
		{
			txt,
			asset,
			texture
		};

		static string Select(const SearchType& searchType);
	};
}