//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

#pragma once

#ifndef CORE_INPUT_H
#define CORE_INPUT_H

//external
#include <glad.h>

//engine
#include "render.h"

#include <string>
#include <unordered_map>

using namespace std;
using namespace Graphics;

namespace Core
{
	class InputManager
	{
	public:
		enum class Key : char
		{
			Escape,
			Enter,
			Forwards,
			Backwards,
			Left,
			Right
		};

		static inline bool firstMouse = true;
		static inline float lastX = Render::SCR_WIDTH / 2.0f;
		static inline float lastY = Render::SCR_HEIGHT / 2.0f;

		static inline unordered_map<Key, int> key;

		static void InputSetup();
		static void ProcessInput(GLFWwindow* window);
		static void Mouse_Callback(GLFWwindow* window, double xPos, double yPos);
		static void Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);
	};
}

#endif