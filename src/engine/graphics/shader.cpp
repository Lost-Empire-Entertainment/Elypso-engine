//external
#include "glad.h"

//engine
#include "shader.h"
#include "console.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Graphics
{
	Shader::Shader(const char* vertexPath, const char* fragmentPath)
	{
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        //ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            //open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            //read file's buffer contents into streams
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
            Core::Console::ConsoleManager::WriteConsoleMessage(
                Core::Console::ConsoleManager::Caller::SHADER,
                Core::Console::ConsoleManager::Type::ERROR,
                "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " +
                std::string(e.what()) + "\n\n");
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        //compile shaders
        unsigned int vertex, fragment;
        //vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        //fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        //shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");
        //delete shaders as they are no longer needed
        glDeleteShader(vertex);
        glDeleteShader(fragment);
	}

	void Shader::Use()
	{
        glUseProgram(ID);
	}

	void Shader::SetBool(const std::string &name, bool value) const
	{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void Shader::SetInt(const std::string& name, int value) const
	{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void Shader::SetFloat(const std::string& name, float value) const
	{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::CheckCompileErrors(unsigned int shader, std::string type)
	{
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Core::Console::ConsoleManager::WriteConsoleMessage(
                    Core::Console::ConsoleManager::Caller::SHADER,
                    Core::Console::ConsoleManager::Type::ERROR,
                    "ERROR::SHADER_COMPILATION_ERROR of type: " +
                    std::string(type) + " " +
                    std::string(infoLog) + "\n\n");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Core::Console::ConsoleManager::WriteConsoleMessage(
                    Core::Console::ConsoleManager::Caller::SHADER,
                    Core::Console::ConsoleManager::Type::ERROR,
                    "ERROR::PROGRAM_LINKING_ERROR of type: " +
                    std::string(type) + " " +
                    std::string(infoLog) + "\n\n");
            }
        }
	}
}