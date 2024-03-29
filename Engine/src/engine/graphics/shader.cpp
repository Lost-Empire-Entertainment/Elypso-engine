//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>

//external
#include "glad.h"

//engine
#include "console.hpp"
#include "shader.hpp"
#include "stringUtils.hpp"

using std::cout;
using std::endl;
using std::ios_base;
using std::ifstream;
using std::stringstream;
using std::filesystem::absolute;
using std::to_string;
using std::vector;

using Utils::String;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	Shader::Shader(const string& vertexPath, const string& fragmentPath)
	{
        if (vertexPath != ""
            && fragmentPath != "")
        {
            vector<string> vertSplit = String::Split(vertexPath, '/');
            vector<string> fragSplit = String::Split(fragmentPath, '/');

            ConsoleManager::WriteConsoleMessage(
                Caller::SHADER,
                Type::DEBUG,
                "Initializing " + vertSplit.back() + " and " + fragSplit.back() + ".\n");

            string vertexCode;
            string fragmentCode;
            ifstream vShaderFile;
            ifstream fShaderFile;
            //ensure ifstream objects can throw exceptions:
            vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
            fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
            try
            {
                //open files
                vShaderFile.open(absolute(vertexPath).string());
                fShaderFile.open(absolute(fragmentPath).string());

                if (!vShaderFile.is_open()
                    || !fShaderFile.is_open())
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::SHADER,
                        Type::EXCEPTION,
                        "ERROR::SHADER::FILE_NOT_OPEN: \nVertex: " + absolute(vertexPath).string() +
                        "\nFragment: " + absolute(fragmentPath).string() + "\n\n");
                    return;
                }

                stringstream vShaderStream, fShaderStream;
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
            catch (const ios_base::failure& e)
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::SHADER,
                    Type::EXCEPTION,
                    "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " +
                    to_string(e.code().value()) +
                    "\nVertex: " + absolute(vertexPath).string() +
                    "\nFragment: " + absolute(fragmentPath).string() + "\n\n");
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

            ConsoleManager::WriteConsoleMessage(
                Caller::SHADER,
                Type::DEBUG,
                "Successfully initialized " + vertSplit.back() + " and " + fragSplit.back() + " with ID " + to_string(ID) + "!\n\n");
        }
	}

	void Shader::Use() const
	{
        glUseProgram(ID);
	}

	void Shader::SetBool(const string &name, bool value) const
	{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void Shader::SetInt(const string& name, int value) const
	{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void Shader::SetFloat(const string& name, float value) const
	{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

    void Shader::SetVec2(const string& name, const vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec2(const string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void Shader::SetVec3(const string& name, const vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec3(const string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void Shader::SetVec4(const string& name, const vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec4(const string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void Shader::SetMat2(const string& name, const mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::SetMat3(const string& name, const mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::SetMat4(const string& name, const mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

	void Shader::CheckCompileErrors(GLuint shader, string type)
	{
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                ConsoleManager::WriteConsoleMessage(
                    Caller::SHADER,
                    Type::EXCEPTION,
                    "ERROR::SHADER_COMPILATION_ERROR of type: " +
                    string(type) + " " +
                    string(infoLog) + "\n\n");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                ConsoleManager::WriteConsoleMessage(
                    Caller::SHADER,
                    Type::EXCEPTION,
                    "ERROR::PROGRAM_LINKING_ERROR of type: " +
                    string(type) + " " +
                    string(infoLog) + "\n\n");
            }
        }
	}
}