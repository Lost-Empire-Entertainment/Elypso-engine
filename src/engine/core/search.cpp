//engine
#include "search.h"

#include <filesystem>

using namespace std;
using namespace std::filesystem;

namespace Core
{
	string Search::SearchByParent(const string& targetFolderName)
	{
		//get the current working directory
		path currentPath = current_path();
		path current = currentPath;

		//iterate until we reach the root directory
		while (!current.empty())
		{
			//check if the target folder exists in the current directory
			if (is_directory(current / targetFolderName))
			{
				return (current / targetFolderName).string();
			}

			current = current.parent_path();
		}

		//if no folder was found
		return "";
	}
}