//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

//engine
#include "gameobject.hpp"
#include "input.hpp"
#include "shader.hpp"

#include <vector>

using std::vector;
using glm::vec3;
using glm::mat4;

namespace Graphics
{
	class Render
	{
	public:
		static inline unsigned int SCR_WIDTH = 1280;
		static inline unsigned int SCR_HEIGHT = 720;

		static inline unsigned int VBO, VAO, cubeVAO, lightCubeVAO, EBO;

		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);
		static inline float shininess = 32;

		static inline vec3 directionalDirection = vec3(-0.2f, -1.0f, -0.3f);
		static inline vec3 directionalDiffuse = vec3(0.4f, 0.4f, 0.4f);
		static inline float directionalIntensity = 1.0f;

		static inline vec3 spotDiffuse = vec3(1.0f, 1.0f, 1.0f);
		static inline float spotIntensity = 1.0f;
		static inline float spotDistance = 1.0f;
		static inline float spotInnerAngle = 12.5f;
		static inline float spotOuterAngle = 17.5f;

		static inline vec3 pointDiffuse = vec3(0.8f, 0.8f, 0.8f);
		static inline float pointIntensity = 1.0f;
		static inline float pointDistance = 1.0f;

		static inline float cubeSpeedMultiplier = 1.0f;
		static inline float lampOrbitRange = 5;
		static inline float cubeWiggleHeight = 2;
		static inline float cubeWiggleSpeed = 1;

		static inline bool useMonitorRefreshRate = true;

		static inline GLFWwindow* window;
		static inline float fov;
		static inline float aspectRatio;
		static inline int windowedWidth;
		static inline int windowedHeight;
		static inline float nearClip;
		static inline float farClip;

		static inline mat4 projection;
		static inline mat4 view;

		static Core::Input camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void WindowLoop();
	private:
		static inline vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);

		static inline int SCR_POS_X;
		static inline int SCR_POS_Y;

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}