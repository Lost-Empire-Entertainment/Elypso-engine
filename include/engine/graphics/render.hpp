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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

//engine
#include "input.hpp"
#include "shader.hpp"

using glm::vec3;

namespace Graphics
{
	class Render
	{
	public:
		static inline unsigned int SCR_WIDTH = 1280;
		static inline unsigned int SCR_HEIGHT = 720;

		static inline unsigned int VBO, VAO, cubeVAO, lightCubeVAO, EBO;

		static inline vec3 directionalLightAngle = vec3(-0.2f, -0.2f, -0.2f);
		static inline float lightConstant = 1.0f;
		static inline float lightLinear = 0.09f;
		static inline float lightQuadratic = 0.032f;
		static inline float spotlightAngle = 12.5f;

		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static inline vec3 cubeColor = vec3(1.0f, 1.0f, 1.0f);
		static inline float cubeColorStrength = 1.0f;

		static inline vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
		static inline float ambientColorStrength = 0.2f;

		static inline vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);
		static inline float shininess = 32;

		static inline float cubeSpeedMultiplier = 0.005f;
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

		static Core::Input camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void WindowLoop();
	private:
		static inline vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

		static inline int SCR_POS_X;
		static inline int SCR_POS_Y;

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}