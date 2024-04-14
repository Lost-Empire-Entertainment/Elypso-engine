//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>

//external
#include "glad.h"

//game
#include "shader.hpp"
#include "stringUtils.hpp"

using std::cout;
using std::endl;
using std::ios_base;
using std::ifstream;
using std::stringstream;
using std::to_string;
using std::vector;
using std::filesystem::absolute;
using std::filesystem::path;

using Utils::String;

namespace Graphics
{
    Shader Shader::LoadShader(const string& vertexPath, const string& fragmentPath)
    {
        Shader shader{};

        path vertexStemPath = vertexPath;
        string vertexStemExtension = vertexStemPath.extension().string();
        vertexStemPath = vertexStemPath.stem();

        path fragmentStemPath = fragmentPath;
        string fragmentStemExtension = fragmentStemPath.extension().string();
        fragmentStemPath = fragmentStemPath.stem();

        string shaderKey = vertexStemPath.string() + vertexStemExtension + "|" + fragmentStemPath.string() + fragmentStemExtension;

        auto it = shaders.find(shaderKey);
        if (it != shaders.end())
        {
            shader.ID = it->second;
            return shader;
        }

        if (vertexPath != ""
            && fragmentPath != "")
        {
            vector<string> vertSplit = String::Split(vertexPath, '/');
            vector<string> fragSplit = String::Split(fragmentPath, '/');

            //cout << "Initializing " << vertSplit.back() << " and " << fragSplit.back() << ".\n";

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
                    cout << "ERROR::SHADER::FILE_NOT_OPEN: \nVertex: " << absolute(vertexPath).string() <<
                        "\nFragment: " << absolute(fragmentPath).string() << "\n\n";
                    return shader;
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
                cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " <<
                    to_string(e.code().value()) <<
                    "\nVertex: " << absolute(vertexPath).string() <<
                    "\nFragment: " << absolute(fragmentPath).string() << "\n\n";
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
            shader.ID = glCreateProgram();
            glAttachShader(shader.ID, vertex);
            glAttachShader(shader.ID, fragment);
            glLinkProgram(shader.ID);
            CheckCompileErrors(shader.ID, "PROGRAM");
            //delete shaders as they are no longer needed
            glDeleteShader(vertex);
            glDeleteShader(fragment);

            shaders.emplace(shaderKey, shader.ID);

            //cout << "Successfully initialized " << vertSplit.back() << " and " << fragSplit.back() << " with ID " << to_string(ID) << "!\n\n";

            return shader;
        }

        return shader;
    }

    void Shader::Use() const
    {
        glUseProgram(ID);
    }

    void Shader::SetBool(const string& name, bool value) const
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
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
                    string(type) << " " <<
                    string(infoLog) << "\n\n";
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " <<
                    string(type) << " " <<
                    string(infoLog) << "\n\n";
            }
        }
    }
}