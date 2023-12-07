#pragma once

#ifndef CORE_SEARCH_H
#define CORE_SEARCH_H

#include <filesystem>

namespace fs = std::filesystem;

namespace Core
{
	class Search
	{
	public:
		static std::string SearchByParent(const std::string& targetFolderName);
	};
}

#endif