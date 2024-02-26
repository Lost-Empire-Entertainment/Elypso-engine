//<Elypso hub>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine 

#pragma once

#include <filesystem>
#include <string>

//external
#include "glfw3.h"
#include "glm.hpp"
#include "imgui.h"

using std::string;
using std::filesystem::path;
using glm::vec3;

class Core
{
public:
	static inline GLFWwindow* window;

	static inline unsigned int SCR_WIDTH = 1280;
	static inline unsigned int SCR_HEIGHT = 720;

	static inline int windowedWidth;
	static inline int windowedHeight;

	static inline float aspectRatio;

	static inline path defaultPath;
	static inline path docsPath;
	static inline path projectsFilePath;
	static inline path configFilePath;
	static inline path enginePath;

	static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

	static void Initialize();
	static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
	static void Render();
	static void Shutdown();
};