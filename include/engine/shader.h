#pragma once

#include "glfw3.h"

enum ShaderState {
	vertex,
	fragment,
	link_shaders,
	vertex_data_and_buffers
};

class ShaderManager
{
public:
	static inline bool shaderSetupSuccess;

	static inline int success;
	static inline char infoLog[512];

	static inline unsigned int VAO, VBO, shaderProgram, vertexShader, fragmentShader;

	static inline const char* vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPos, 1.0);\n"
		"}\0";
	static inline const char* fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;"
		"uniform vec4 ourColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = ourColor;\n"
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