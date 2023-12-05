//external
#include "glad.h"
#include "magic_enum.hpp"

//engine
#include "shader.h"
#include "render.h"
#include "console.h"

#include <sstream>

namespace Graphics
{
	void ShaderManager::ShaderSetup()
	{
		ShaderManager::SetUpVertexShader();
	}
	void ShaderManager::SetUpVertexShader()
	{
		Core::ConsoleManager::WriteConsoleMessage(
			Core::ConsoleManager::Caller::SHADER,
			Core::ConsoleManager::Type::INFO,
			"Initializing vertex shader...\n");
		ShaderManager::vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(
			ShaderManager::vertexShader,
			1,
			&ShaderManager::vertexShaderSource,
			NULL);
		glCompileShader(ShaderManager::vertexShader);
		if (!ShaderManager::FoundShaderCompileErrors(ShaderManager::ShaderState::vertex))
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::SUCCESS,
				"Vertex shader initialized successfully!\n\n");
		}
		else
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::ERROR,
				"Vertex shader compilation failed!\n\n");
			return;
		}
		ShaderManager::SetUpFragmentShader();
	}
	void ShaderManager::SetUpFragmentShader()
	{
		Core::ConsoleManager::WriteConsoleMessage(
			Core::ConsoleManager::Caller::SHADER,
			Core::ConsoleManager::Type::INFO,
			"Initializing fragment shader...\n");
		ShaderManager::fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(
			ShaderManager::fragmentShader,
			1,
			&ShaderManager::fragmentShaderSource,
			NULL);
		glCompileShader(ShaderManager::fragmentShader);
		if (!ShaderManager::FoundShaderCompileErrors(ShaderManager::ShaderState::fragment))
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::SUCCESS,
				"Fragment shader initialized successfully!\n\n");
		}
		else
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::ERROR,
				"Fragment shader compilation failed!\n\n");
			return;
		}
		ShaderManager::LinkShaders();
	}
	void ShaderManager::LinkShaders()
	{
		Core::ConsoleManager::WriteConsoleMessage(
			Core::ConsoleManager::Caller::SHADER,
			Core::ConsoleManager::Type::INFO,
			"Linking shaders...\n");
		//link shaders
		ShaderManager::shaderProgram = glCreateProgram();
		glAttachShader(ShaderManager::shaderProgram, ShaderManager::vertexShader);
		glAttachShader(ShaderManager::shaderProgram, ShaderManager::fragmentShader);
		glLinkProgram(ShaderManager::shaderProgram);
		//clean up shaders
		glDeleteShader(ShaderManager::vertexShader);
		glDeleteShader(ShaderManager::fragmentShader);
		if (!ShaderManager::FoundShaderCompileErrors(ShaderManager::ShaderState::link_shaders))
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::SUCCESS,
				"Shaders linked successfully!\n\n");
		}
		else
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::ERROR,
				"Shader linking failed!\n\n");
			return;
		}
		ShaderManager::SetUpVertexDataAndBuffers();
	}
	void ShaderManager::SetUpVertexDataAndBuffers()
	{
		Core::ConsoleManager::WriteConsoleMessage(
			Core::ConsoleManager::Caller::SHADER,
			Core::ConsoleManager::Type::INFO,
			"Setting up vertex data and buffers...\n");

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

		glUseProgram(ShaderManager::shaderProgram);

		if (!ShaderManager::FoundShaderCompileErrors(ShaderManager::ShaderState::vertex_data_and_buffers))
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::SUCCESS,
				"Vertex data and buffer setup was successful!\n\n");
		}
		else
		{
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::SHADER,
				Core::ConsoleManager::Type::ERROR,
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
			Core::ConsoleManager::WriteConsoleMessage(
				Core::ConsoleManager::Caller::ENGINE,
				Core::ConsoleManager::Type::ERROR,
				oss.str());
			return true;
			break;
		case ShaderManager::ShaderState::vertex:
			glGetShaderiv(ShaderManager::vertexShader, GL_COMPILE_STATUS, &ShaderManager::success);
			if (!ShaderManager::success)
			{
				glGetShaderInfoLog(
					ShaderManager::vertexShader,
					512,
					NULL,
					ShaderManager::infoLog);
				message = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n\n" + std::string(ShaderManager::infoLog);
				Core::ConsoleManager::WriteConsoleMessage(
					Core::ConsoleManager::Caller::SHADER,
					Core::ConsoleManager::Type::ERROR,
					message);
				return true;
			}
			break;
		case ShaderManager::ShaderState::fragment:
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
				Core::ConsoleManager::WriteConsoleMessage(
					Core::ConsoleManager::Caller::SHADER,
					Core::ConsoleManager::Type::ERROR,
					message);
				return true;
			}
			break;
		case ShaderManager::ShaderState::link_shaders:
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
				Core::ConsoleManager::WriteConsoleMessage(
					Core::ConsoleManager::Caller::SHADER,
					Core::ConsoleManager::Type::ERROR,
					message);
				return true;
			}
			break;
		case ShaderManager::ShaderState::vertex_data_and_buffers:
			error = glGetError();
			if (ShaderManager::error != GL_NO_ERROR)
			{
				message = "OpenGL Error: " + std::to_string(ShaderManager::error) + "\n\n";
				Core::ConsoleManager::WriteConsoleMessage(
					Core::ConsoleManager::Caller::SHADER,
					Core::ConsoleManager::Type::ERROR,
					message);
				return true;
			}
			break;
		}
		return false;
	}
}