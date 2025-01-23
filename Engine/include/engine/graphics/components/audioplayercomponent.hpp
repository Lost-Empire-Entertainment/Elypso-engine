//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

//engine
#include "component.hpp"

namespace Graphics::Components
{
	using std::string;

	class AudioPlayerComponent : public Component
	{
	public:
		enum class AudioType
		{
			music,
			sfx
		};

		static inline bool isPlaying;
		static inline bool is2D;

		static inline float volume;
		static inline float minVolume;
		static inline float maxVolume;

		static inline string audioFilePath;

		/// <summary>
		/// Import an audio file with a specific path.
		/// </summary>
		static bool Import(const string& path);

		/// <summary>
		/// Play an audio file with a specific path.
		/// </summary>
		static bool Play(const string& path);

		/// <summary>
		/// Remove an audio file from the project with a specific path.
		/// </summary>
		static void Delete(const string& path);
	};
}