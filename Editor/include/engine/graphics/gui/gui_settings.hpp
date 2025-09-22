//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <map>
#include <string>

namespace Graphics::GUI
{
	using std::map;
	using std::string;

	class GUISettings
	{
	public:
		static inline map<string, string> skyboxTextures;

		static inline bool renderSettings;
		static inline string playerCameraName;
		static void RenderSettings();

	private:
		enum class NameDenyReason
		{
			invalidCharacter,
			invalidName
		};
		static inline NameDenyReason nameDenyReason;

		static constexpr int bufferSize = 32;
		static inline char gameNameChar[bufferSize];

		static void InputSettings();
		static void GUIStyleSettings();
		static void GraphicsSettings();
		static void PhysicsSettings();
		static void GameSettings();
	};
}
#endif