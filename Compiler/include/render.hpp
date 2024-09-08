//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics
{
	class Render
	{
	public:
		static void RenderInitialize();
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();

		static void RenderLoop();
		static void RenderShutdown();
	};
}