#pragma once

//external
#include "glad.h"

//engine
#include "console.h"

#include <string>

namespace Graphics
{
	class ShaderManager
	{
	public:
		enum class ShaderState : char
		{
			vertex,
			fragment,
			link_shaders,
			vertex_data_and_buffers
		};

		static inline bool shaderSetupSuccess;
		static inline int success;
		static inline char infoLog[512];
		static inline unsigned int VAO, VBO, shaderProgram, vertexShader, fragmentShader;
		static inline const char* vertexShaderSource =
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aColor;\n"
			"out vec3 ourColor;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4(aPos, 1.0);\n"
			"	ourColor = aColor;\n"
			"}\0";
		static inline const char* fragmentShaderSource =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec3 ourColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4(ourColor, 1.0); \n"
			"}\n\0";

		static inline GLenum error;
		static inline GLuint framebuffer = 0;
		static inline GLuint textureColorbuffer = 0;
		static void ShaderSetup();
		static void SetUpVertexShader();
		static void SetUpFragmentShader();
		static void LinkShaders();
		static void SetUpVertexDataAndBuffers();
		static bool FoundShaderCompileErrors(ShaderState shaderState);
	};
}