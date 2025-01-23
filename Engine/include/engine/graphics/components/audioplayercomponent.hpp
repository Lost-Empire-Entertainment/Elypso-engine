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
		AudioPlayerComponent(
			const bool& isPlaying = false,
			const bool& isPaused = false,
			const bool& is3D = false,
			const float& volume = 50.0f,
			const float& minVolume = 0.0f, 
			const float& maxVolume = 100.0f) :
			isPlaying(isPlaying),
			isPaused(isPaused),
			is3D(is3D),
			volume(volume),
			minVolume(minVolume),
			maxVolume(maxVolume) {}

		void SetPlayState(bool state) { isPlaying = state; }
		void SetPauseState(bool state) { isPaused = state; }
		void Set2DState(bool state) { is3D = state; }
		void SetVolume(float newVolume) { volume = newVolume; }
		void SetPath(string newPath) { audioFilePath = newPath; }

		bool IsPlaying() const { return isPlaying; }
		bool IsPaused() const { return isPaused; }
		bool Is3D() const { return is3D; }
		float GetVolume() const { return volume; }
		float GetMinVolume() const { return minVolume; }
		float GetMaxVolume() const { return maxVolume; }
		string GetPath() const { return audioFilePath; }

		type_index GetType() const override { return typeid(AudioPlayerComponent); }
	private:
		bool isPlaying;
		bool isPaused;
		bool is3D;
		float volume;
		float minVolume;
		float maxVolume;
		string audioFilePath;
	};
}