//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class Audio
	{
	public:
		static bool Initialize();

		static bool Import(const string& path);

		/// <summary>
		/// Start audio file from the very beginning.
		/// </summary>
		static bool Play(const string& path);
		/// <summary>
		/// Stop playing audio file and reset its position to beginning.
		/// </summary>
		static bool Stop(const string& path);

		/// <summary>
		/// Pause playing audio file but remember its position.
		/// </summary>
		static bool Pause(const string& path);
		/// <summary>
		/// Continue playing audio file from where it was left off.
		/// </summary>
		static bool Continue(const string& path);

		static void SetVolume(const string& path, float currVolume);

		static bool Delete(const string& path);

		static bool IsImported(const string& path);

		static void Shutdown();
	};
}