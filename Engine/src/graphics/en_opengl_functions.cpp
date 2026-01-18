//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "graphics/en_opengl_functions.hpp"

namespace ElypsoEngine::Graphics::OpenGLFunctions
{
	static const GL_Engine* glEngine{};

	void OpenGL_Functions_Engine::SetGLEngine(const GL_Engine* gl) { glEngine = gl; }

	const GL_Engine* OpenGL_Functions_Engine::GetGLEngine() { return glEngine; }
}