//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core.hpp"

using Core::GameCore;

int main()
{
	GameCore::InitializeGame();
	GameCore::RunGame();
	return 0;
}