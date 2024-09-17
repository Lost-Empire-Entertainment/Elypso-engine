//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core.hpp"

using Core::Compiler;

int main()
{
	Compiler::MainInitialize();
	Compiler::MainLoop();

	return 0;
}