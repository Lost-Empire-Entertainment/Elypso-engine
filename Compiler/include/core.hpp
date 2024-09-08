//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Core
{
	class Compiler
	{
	public:
		static void IsAlreadyRunning();

		static void MainInitialize();
		static void MainLoop();
		static void MainShutdown();
	};
}