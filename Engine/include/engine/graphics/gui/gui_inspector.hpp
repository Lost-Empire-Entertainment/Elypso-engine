//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUIInspector
	{
	public:
		static void RenderInspector();
	private:
		static constexpr int bufferSize = 32;

		static inline char inputTextBuffer_objName[bufferSize];

		static void PermanentComponents();
	};
}