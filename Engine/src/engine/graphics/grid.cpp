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

//external
#include "glm/gtc/matrix_transform.hpp"
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.hpp"
#include "core.hpp"

#include <vector>

using std::vector;
using glm::vec3;
using glm::rotate;
using glm::radians;

using Core::Engine;
using Graphics::Shader;

//engine
#include "grid.hpp"

namespace Graphics
{
	GLuint vao, vbo;

	void Grid::SetGridSize(float assignedGridSize)
	{
		gridSize = assignedGridSize;
	}
	void Grid::SetRenderDistance(float assignedRenderDistance)
	{
		renderDistance = assignedRenderDistance;
	}

	float Grid::GetGridSize()
	{
		return gridSize;
	}
	float Grid::GetRenderDistance()
	{
		return renderDistance;
	}

	void Grid::InitializeGridX()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		vector<float> vertices;
		int numVertices = static_cast<int>(renderDistance / gridSize) + 1;
		// Generate lines along the x-axis
		for (int i = 0; i < numVertices; ++i)
		{
			float x = -renderDistance / 2.0f + i * gridSize;

			vertices.push_back(x);
			vertices.push_back(-renderDistance / 2.0f);
			vertices.push_back(0.0f);

			vertices.push_back(x);
			vertices.push_back(renderDistance / 2.0f);
			vertices.push_back(0.0f);
		}

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		gridShader = Shader(
			Engine::enginePath + "/shaders/Grid.vert",
			Engine::enginePath + "/shaders/Grid.frag");

	}
	void Grid::InitializeGridY()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		vector<float> vertices;
		int numVertices = static_cast<int>(renderDistance / gridSize) + 1;
		// Generate lines along the x-axis
		for (int i = 0; i < numVertices; ++i)
		{
			float x = -renderDistance / 2.0f + i * gridSize;

			vertices.push_back(x);
			vertices.push_back(-renderDistance / 2.0f);
			vertices.push_back(0.0f);

			vertices.push_back(x);
			vertices.push_back(renderDistance / 2.0f);
			vertices.push_back(0.0f);
		}

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		gridShader = Shader(
			Engine::enginePath + "/shaders/Grid.vert",
			Engine::enginePath + "/shaders/Grid.frag");

	}

	void Grid::RenderGridX(mat4 view, mat4 projection)
	{
		glBindVertexArray(vao);

		mat4 model = mat4(1.0f);
		mat4 rotation = rotate(mat4(1.0f), radians(90.0f), vec3(-1.0f, 0.0f, 0.0f));
		mat4 mvp = projection * view * rotation * model;

		gridShader.Use();
		gridShader.SetMat4("uMVP", mvp);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(renderDistance / gridSize) * 1);

		glBindVertexArray(0);
	}
	void Grid::RenderGridY(mat4 view, mat4 projection)
	{
		glBindVertexArray(vao);

		mat4 model = mat4(1.0f);
		mat4 rotation = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		mat4 mvp = projection * view * rotation * model;

		gridShader.Use();
		gridShader.SetMat4("uMVP", mvp);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(renderDistance / gridSize) * 1);

		glBindVertexArray(0);
	}
}