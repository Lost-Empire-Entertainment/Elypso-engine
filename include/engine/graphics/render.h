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

#ifndef GRAPHICS_RENDER_H
#define GRAPHICS_RENDER_H

//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.h"

#include <string>

using namespace glm;

namespace Graphics
{
	class Render
	{
	public:
		static inline unsigned int VBO, VAO, EBO;

		static inline GLFWwindow* window;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void Shutdown();
		static void WindowLoop();
	private:

		static Shader* shader;

		static inline const unsigned int SCR_WIDTH = 1200;
		static inline const unsigned int SCR_HEIGHT = 1200;

		static inline vec3 cubePositions[10];

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}

#endif