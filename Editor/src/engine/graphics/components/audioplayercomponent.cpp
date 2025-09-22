//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <algorithm>

//engine
#include "audioplayercomponent.hpp"
#include "audio.hpp"

using std::clamp;

using Core::Audio;

namespace Graphics::Components
{
	void AudioPlayerComponent::SetVolume(float newVolume)
	{
		const shared_ptr<GameObject>& owner = GetOwner();

		volume = newVolume;
		Audio::SetVolume(audioFileName, volume, owner);
	}
	void AudioPlayerComponent::SetMinRange(float newMinRange)
	{
		const shared_ptr<GameObject>& owner = GetOwner();

		minRange = newMinRange;
		Audio::SetMinRange(audioFileName, minRange, owner);
	}
	void AudioPlayerComponent::SetMaxRange(float newMaxRange)
	{
		const shared_ptr<GameObject>& owner = GetOwner();

		maxRange = newMaxRange;
		Audio::SetMaxRange(audioFileName, maxRange, owner);
	}
	void AudioPlayerComponent::Set3DState(bool newState)
	{
		is3D = newState;
	}
}