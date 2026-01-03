//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

namespace KalaUI::OpenGL
{
	using std::string;
	
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::vec2;
	
	class LIB_API OpenGL_Manager
	{
	public:
		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	
		static bool IsContextValid(uintptr_t glContext);
	
		static void Update(
			const mat4& projection,
			f32 viewportHeight);
	};
}