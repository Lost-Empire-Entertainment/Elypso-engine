//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Audio
{
	class AudioCore
	{
	public:
		AudioCore();
		~AudioCore();

		static bool Initialize();

		static bool Import(const string& path);
		static bool Play(const string& path);

		static void Shutdown();
	};
}