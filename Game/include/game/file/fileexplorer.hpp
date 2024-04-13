//Copyright(C) < 2024 > < Lost Empire Entertainment >

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