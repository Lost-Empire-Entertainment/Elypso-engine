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

//external
#include "glm.hpp"

//engine
#include "shader.hpp"

using Graphics::Shader;

using glm::mat4;

namespace Graphics
{
	class Grid
	{
	public:
		static void SetGridSize(float gridSize);
		static void SetRenderDistance(float renderDistance);

		static float GetGridSize();
		static float GetRenderDistance();

		static void InitializeGridX();
		static void RenderGridX(mat4 view, mat4 projection);

		static void InitializeGridY();
		static void RenderGridY(mat4 view, mat4 projection);
	private:
		static inline Shader gridShader;
		static inline float gridSize = 1.0f;
		static inline float renderDistance = 500.0f;
	};
}