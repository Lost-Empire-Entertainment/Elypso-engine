//engine
#include "search.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Core
{
	std::string Search::SearchByParent(const std::string& targetFolderName)
	{
		//get the current working directory
		fs::path currentPath = fs::current_path();
		fs::path current = currentPath;

		//iterate until we reach the root directory
		while (!current.empty())
		{
			//check if the target folder exists in the current directory
			if (fs::is_directory(current / targetFolderName))
			{
				return (current / targetFolderName).string();
			}

			current = current.parent_path();
		}

		//if no folder was found
		return "";
	}
}