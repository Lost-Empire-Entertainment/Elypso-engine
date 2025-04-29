//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "g_core.hpp"

using GameTemplate::G_Core;

int main()
{
	G_Core::Initialize();
	G_Core::Run();
	G_Core::Shutdown();
	return 0;
}