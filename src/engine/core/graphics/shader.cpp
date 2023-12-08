//external
#include "glad.h"

//engine
#include "shader.h"
#include "console.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Core::Graphics
{
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
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

	void Shader::Use() const
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

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

	void Shader::CheckCompileErrors(GLuint shader, std::string type)
	{
        GLint success;
        GLchar infoLog[1024];
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