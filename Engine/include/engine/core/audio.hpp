//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "glm.hpp"

namespace Core
{
	using std::string;
	using glm::vec3;

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

		/// <summary>
		/// Set audio player volume.
		/// </summary>
		/// <param name="path"></param>
		static void SetVolume(const string& path, float currVolume);
		/// <summary>
		/// Toggle audio player positioning, 2D is absolute, 3D is relative.
		/// </summary>
		static bool Set3DState(const string& path, bool state);

		/// <summary>
		/// Update position where player hears audio.
		/// </summary>
		static void UpdateListenerPosition(const vec3& pos);
		/// <summary>
		/// Update position where audio player plays sound.
		/// </summary>
		static bool UpdatePlayerPosition(const string& name, const vec3& pos);

		/// <summary>
		/// Delete this audio player from memory.
		/// </summary>
		/// <param name="path"></param>
		static bool Delete(const string& path);

		/// <summary>
		/// Check if this audio file is imported to memory or not.
		/// </summary>
		static bool IsImported(const string& path);

		static void Shutdown();
	};
}