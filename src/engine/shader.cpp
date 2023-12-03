//external
#include "magic_enum.hpp"
#include "glad.h"

//engine
#include "shader.h"
#include "console.h"

#include <iostream>
#include <string>
#include <sstream>

void ShaderManager::ShaderSetup()
{
	ShaderManager::SetUpVertexShader();
}

void ShaderManager::SetUpVertexShader()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::MessageType::SHADER, 
		ConsoleManager::ErrorType::INFO, 
		"Initializing vertex shader...\n");

	ShaderManager::vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(
		ShaderManager::vertexShader, 
		1, 
		&ShaderManager::vertexShaderSource, 
		NULL);
	glCompileShader(ShaderManager::vertexShader);

	if (!ShaderManager::FoundShaderCompileErrors(vertex))
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER,
			ConsoleManager::ErrorType::SUCCESS,
			"Vertex shader initialized successfully!\n\n");
	}
	else
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER,
			ConsoleManager::ErrorType::ERROR,
			"Vertex shader compilation failed!\n\n");
		return;
	}

	ShaderManager::SetUpFragmentShader();
}
void ShaderManager::SetUpFragmentShader()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::MessageType::SHADER,
		ConsoleManager::ErrorType::INFO,
		"Initializing fragment shader...\n");

	ShaderManager::fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(
		ShaderManager::fragmentShader, 
		1, 
		&ShaderManager::fragmentShaderSource, 
		NULL);
	glCompileShader(ShaderManager::fragmentShader);

	if (!ShaderManager::FoundShaderCompileErrors(fragment))
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::SUCCESS, 
			"Fragment shader initialized successfully!\n\n");
	}
	else
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::ERROR, 
			"Fragment shader compilation failed!\n\n");
		return;
	}

	ShaderManager::LinkShaders();
}
void ShaderManager::LinkShaders()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::MessageType::SHADER, 
		ConsoleManager::ErrorType::INFO, 
		"Linking shaders...\n");

	//link shaders
	ShaderManager::shaderProgram = glCreateProgram();
	glAttachShader(ShaderManager::shaderProgram, ShaderManager::vertexShader);
	glAttachShader(ShaderManager::shaderProgram, ShaderManager::fragmentShader);
	glLinkProgram(ShaderManager::shaderProgram);

	//clean up shaders
	glDeleteShader(ShaderManager::vertexShader);
	glDeleteShader(ShaderManager::fragmentShader);

	if (!ShaderManager::FoundShaderCompileErrors(link_shaders))
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::SUCCESS, 
			"Shaders linked successfully!\n\n");
	}
	else
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::ERROR, 
			"Shader linking failed!\n\n");
		return;
	}

	ShaderManager::SetUpVertexDataAndBuffers();
}
void ShaderManager::SetUpVertexDataAndBuffers()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::MessageType::SHADER, 
		ConsoleManager::ErrorType::INFO, 
		"Setting up vertex data and buffers...\n");

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	//set up vertex data and buffers and configure vertex attributes
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, //left
		0.5f, -0.5f, 0.0f,  //right
		0.0f, 0.5f, 0.0f    //top
	};

	glGenVertexArrays(1, &ShaderManager::VAO);
	glGenBuffers(1, &ShaderManager::VBO);

	//bind the Vertex Array Object first, 
	//then bind and set vertex buffers, 
	//and then configure vertex attributes
	glBindVertexArray(ShaderManager::VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(
		0, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		3 * sizeof(float), 
		(void*)0);
	glEnableVertexAttribArray(0);

	//the call to glVertexAttribPointer registered VBO
	//as the vertex attributes bound vertex buffer object
	//so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//unbind the VAO afterwards so other VAO calls wont accidentally modify this VAO,
	//but this rarely happens. modifying other VAOs requires a call to glBindVertexArray anyways
	//so we generally dont unbind VAOs (nor VBOs) when its not directly necessary
	glBindVertexArray(0);

	//uncomment this call to draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (!ShaderManager::FoundShaderCompileErrors(vertex_data_and_buffers))
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::SUCCESS, 
			"Vertex data and buffer setup was successful!\n\n");
	}
	else
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::ERROR, 
			"Failed to set up vertex data and buffers!\n\n");
		return;
	}

	ShaderManager::shaderSetupSuccess = true;
}

//checks if any errors were found during compilation
bool ShaderManager::FoundShaderCompileErrors(ShaderState state)
{
	std::ostringstream oss;
	std::string message;

	switch (state)
	{
	default:
		oss << "Error: " << magic_enum::enum_name(state) << " is not a valid shader state!\n\n";
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::ENGINE, 
			ConsoleManager::ErrorType::ERROR, 
			oss.str());

		return true;

		break;
	case vertex:
		glGetShaderiv(ShaderManager::vertexShader, GL_COMPILE_STATUS, &ShaderManager::success);
		if (!ShaderManager::success)
		{
			glGetShaderInfoLog(
				ShaderManager::vertexShader, 
				512, 
				NULL, 
				ShaderManager::infoLog);
			message = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n\n" + std::string(ShaderManager::infoLog);
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::MessageType::SHADER, 
				ConsoleManager::ErrorType::ERROR, 
				message);

			return true;
		}
		break;
	case fragment:
		glGetShaderiv(
			ShaderManager::fragmentShader, 
			GL_COMPILE_STATUS, 
			&ShaderManager::success);
		if (!ShaderManager::success)
		{
			glGetShaderInfoLog(
				ShaderManager::fragmentShader, 
				512, 
				NULL, 
				ShaderManager::infoLog);
			message = "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n\n" + std::string(ShaderManager::infoLog);
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::MessageType::SHADER, 
				ConsoleManager::ErrorType::ERROR, 
				message);

			return true;
		}
		break;
	case link_shaders:
		glGetProgramiv(
			ShaderManager::shaderProgram, 
			GL_LINK_STATUS, 
			&ShaderManager::success);
		if (!ShaderManager::success)
		{
			glGetProgramInfoLog(
				ShaderManager::shaderProgram, 
				512, 
				NULL, 
				ShaderManager::infoLog);
			message = "ERROR::SHADER::PROGRAM::LINKING_FAILED\n\n" + std::string(ShaderManager::infoLog);
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::MessageType::SHADER, 
				ConsoleManager::ErrorType::ERROR, 
				message);

			return true;
		}
		break;
	case vertex_data_and_buffers:
		error = glGetError();
		if (ShaderManager::error != GL_NO_ERROR)
		{
			message = "OpenGL Error: " + std::to_string(ShaderManager::error) + "\n\n";
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::MessageType::SHADER, 
				ConsoleManager::ErrorType::ERROR, 
				message);

			return true;
		}

		break;
	}

	return false;
}