//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "audioplayercomponent.hpp"
#include "audio.hpp"

using Core::Audio;

namespace Graphics::Components
{
	bool AudioPlayerComponent::Import(const string& path)
	{
		if (!Audio::Import(path))
		{
			return false;
		}

		return true;
	}

	bool AudioPlayerComponent::Play(const string& path)
	{
		if (!Audio::Play(path))
		{
			return false;
		}

		return true;
	}

	void AudioPlayerComponent::Delete(const string& path)
	{

	}
}