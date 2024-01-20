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

using glm::vec3;

using Graphics::Shader;

namespace Graphics
{
	class Grid
	{
	public:
		static inline vec3 color = vec3(1.0f, 1.0f, 1.0f);
		static inline float transparency = 0.1f;

		static void InitializeGrid();
		static void RenderGrid(const mat4& view, const mat4& projection);
	private:
		static inline GLuint VAO, VBO;
		static inline Shader shader;

		static inline const int lineCount = 250;
		static inline float lineDistance = 1.0f;
		static inline float vertices[lineCount * 4 * 3];
	};
}