//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <cstdlib>
#include <string>

using std::string;
#endif

//engine
#include "browserUtils.hpp"

namespace Utils
{
	void Browser::OpenLink(const char* link)
	{
#ifdef _WIN32
		ShellExecuteA(nullptr, "open", link, nullptr, nullptr, SW_SHOWNORMAL);
#elif __linux__
		string command = "xdg-open \"" + string(link) + "\"";
		system(command.c_str());
#endif
	}
}