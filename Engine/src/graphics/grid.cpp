//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glad.h"

//engine
#include "grid.hpp"
#include "shader.hpp"
#include "core.hpp"

using glm::mat4;

using Graphics::Shader;
using Core::Engine;

namespace Graphics
{
	void Grid::InitializeGrid()
	{
		float lineLength = static_cast<float>(lineCount) / 2;

		for (int i = 0; i < lineCount; ++i)
		{
			float offset = 
				lineDistance * static_cast<float>(i) 
				- (lineDistance * static_cast<float>(lineCount) * 0.5f);

			// horizontal lines
			vertices[i * 4 * 3] = offset;
			vertices[i * 4 * 3 + 1] = 0.0f;
			vertices[i * 4 * 3 + 2] = lineLength;

			vertices[i * 4 * 3 + 3] = offset;
			vertices[i * 4 * 3 + 4] = 0.0f;
			vertices[i * 4 * 3 + 5] = -lineLength;

			// vertical lines
			vertices[i * 4 * 3 + 6] = lineLength;
			vertices[i * 4 * 3 + 7] = 0.0f;
			vertices[i * 4 * 3 + 8] = offset;

			vertices[i * 4 * 3 + 9] = -lineLength;
			vertices[i * 4 * 3 + 10] = 0.0f;
			vertices[i * 4 * 3 + 11] = offset;
		}

		shader = Shader::LoadShader(
			Engine::filesPath + "\\shaders\\Basic.vert",
			Engine::filesPath + "\\shaders\\Basic.frag");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Grid::RenderGrid(const mat4& view, const mat4& projection)
	{
		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		shader.SetFloat("transparency", transparency);
		shader.SetVec3("color", color);

		glBindVertexArray(VAO);

		glDrawArrays(GL_LINES, 0, lineCount * 4);

		glBindVertexArray(0);
	}
}