//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define NOMINMAX
#include <Windows.h>

//engine
#include "browserUtils.hpp"

namespace Utils
{
	void Browser::OpenLink(const char* link)
	{
		ShellExecuteA(nullptr, "open", link, nullptr, nullptr, SW_SHOWNORMAL);
	}
}