//engine
#include "pathUtils.h"

#include <filesystem>

namespace fs = std::filesystem;

std::string PathUtils::GetAbsolutePath(const char* relativePath)
{
	/*
	fs::path basePath = ELYPSO_ENGINE_PATH;

	fs::path absolutePath = (basePath / relativePath).make_preferred();

	return absolutePath.string();
	*/

	return "";
}