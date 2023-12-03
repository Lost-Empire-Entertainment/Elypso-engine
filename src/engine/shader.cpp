//external
#include "glad.h"
#include "magic_enum.hpp"

//engine
#include "shader.h"
#include "render.h"
#include "console.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

void ShaderManager::ShaderSetup()
{
	//call the shader generator to set up the vertex and fragment shaders
	ShaderManager shader(ShaderManager::vertexShader, ShaderManager::fragmentShader);

	//set up vertex data and buffers and configure vertex attributes
	float vertices[] = {
		//positions			//colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, //bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, //bottom left
		0.0f, 0.5f, 0.0f ,  0.0f, 0.0f, 1.0f  //top
	};

	glGenVertexArrays(1, &ShaderManager::VAO);
	glGenBuffers(1, &ShaderManager::VBO);

	//bind the Vertex Array Object first, 
	//then bind and set vertex buffers, 
	//and then configure vertex attributes
	glBindVertexArray(ShaderManager::VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);
	//color attribute
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ShaderManager::shaderSetupSuccess = true;
}

ShaderManager::ShaderManager(const char* vertexPath, const char* fragmentPath)
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::SHADER,
		ConsoleManager::Type::ERROR,
		"Absolute vertex path: " + std::string(vertexPath) + "\n");
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::SHADER,
		ConsoleManager::Type::ERROR,
		"Absolute fragment path: " + std::string(fragmentPath) + "\n");

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read file buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::SHADER,
			ConsoleManager::Type::ERROR,
			"Shader failed to load! " +
			std::string(e.what()));
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	ShaderManager::CheckCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	ShaderManager::CheckCompileErrors(fragment, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	ShaderManager::CheckCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ShaderManager::Use()
{
	glUseProgram(ID);
}

void ShaderManager::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderManager::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderManager::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderManager::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::Caller::SHADER,
				ConsoleManager::Type::ERROR,
				"ERROR::SHADER_COMPILATION_ERROR of type: " +
				type + "\n" +
				infoLog + "\n");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::Caller::SHADER,
				ConsoleManager::Type::ERROR,
				"ERROR::PROGRAM_LINKING_ERROR of type: " +
				type + "\n" +
				infoLog + "\n");
		}
	}
}