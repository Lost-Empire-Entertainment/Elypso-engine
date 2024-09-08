//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "render.hpp"
#include "gui.hpp"

namespace Graphics
{
	void Render::RenderInitialize()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();

		GUI::GUIInitialize();
	}

	void Render::GLFWSetup()
	{

	}

	void Render::WindowSetup()
	{

	}

	void Render::GladSetup()
	{

	}

	void Render::RenderLoop()
	{
		GUI::GUILoop();
	}
}