//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core.hpp"
#include "render.hpp"

using Core::Compiler;
using Graphics::Render;

void Compiler::MainInitialize()
{
	Render::RenderInitialize();
}

void Compiler::IsAlreadyRunning()
{

}

void Compiler::MainLoop()
{
	Render::RenderLoop();
}

void Compiler::MainShutdown()
{
	Render::RenderShutdown();
}