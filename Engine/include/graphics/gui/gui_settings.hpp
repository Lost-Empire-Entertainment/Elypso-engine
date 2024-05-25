//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUISettings
	{
	public:
		static inline bool renderSettings;
		static void RenderSettings();

	private:
		static void InputSettings();
		static void GUIStyleSettings();
		static void OtherSettings();
	};
}