#pragma once

#ifndef CORE_SEARCH_H
#define CORE_SEARCH_H

#include <filesystem>

using namespace std;

namespace Core
{
	class Search
	{
	public:
		/// <summary>
		/// A simple search script that goes up one parent at a time until it reaches the desired folder by name, stops at root folder.
		/// </summary>
		/// <param name="targetFolderName">What is the desired folder name we are searching for?</param>
		/// <returns></returns>
		static string SearchByParent(const string& targetFolderName);
	};
}

#endif