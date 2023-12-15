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

//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.hpp"
#include "input.hpp"

#include <string>
#include <iostream>
#include <filesystem>

using namespace glm;

namespace Graphics
{
	class Render
	{
	public:
		static inline const unsigned int SCR_WIDTH = 1280;
		static inline const unsigned int SCR_HEIGHT = 720;

		static inline unsigned int VBO, VAO, EBO;

		static inline float cameraSpeed;
		static inline vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
		static inline vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
		static inline vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

		static inline bool useMonitorRefreshRate = true;

		static inline GLFWwindow* window;
		static inline float fov = 90;
		static inline float aspectRatio;
		static inline int windowedWidth;
		static inline int windowedHeight;
		static inline float nearClip = 0.001f;
		static inline float farClip = 100.0f;

		static Core::Input camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void Shutdown();
		static void WindowLoop();
	private:

		static Shader* shader;

		static inline vec3 cubePositions[10];

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}