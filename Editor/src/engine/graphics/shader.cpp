//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

//external
#include "glad.h"

//engine
#include "console.hpp"
#include "shader.hpp"
#include "stringutils.hpp"
#include "core.hpp"

using std::cout;
using std::endl;
using std::ios_base;
using std::ifstream;
using std::stringstream;
using std::to_string;
using std::vector;
using std::filesystem::absolute;
using std::filesystem::path;
using std::filesystem::exists;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Core::Engine;

namespace Graphics
{
    unordered_map<string, unsigned int> Shader::shaders;

    Shader Shader::LoadShader(
        const string& vertexPath, 
        const string& fragmentPath,
        const string& geometryPath)
    {
        if (vertexPath == ""
            || !exists(vertexPath))
        {
            string errorMessage = "Vertex shader path '" + vertexPath + "' does not exist or is empty!";
            Engine::CreateErrorPopup(errorMessage.c_str());
        }
        if (fragmentPath == ""
            || !exists(fragmentPath))
        {
            string errorMessage = "Fragment shader path '" + fragmentPath + "' does not exist or is empty!";
            Engine::CreateErrorPopup(errorMessage.c_str());
        }
        if (geometryPath != ""
            && !exists(geometryPath))
        {
            string errorMessage = "Geometry shader path '" + geometryPath + "' does not exist or is empty!";
            Engine::CreateErrorPopup(errorMessage.c_str());
        }

        Shader shader{};

        path vertexStemPath = vertexPath;
        string vertexStemExtension = vertexStemPath.extension().string();
        vertexStemPath = vertexStemPath.stem();

        path fragmentStemPath = fragmentPath;
        string fragmentStemExtension = fragmentStemPath.extension().string();
        fragmentStemPath = fragmentStemPath.stem();

        path geometryStemPath = geometryPath;
        string geometryStemExtension = geometryStemPath.extension().string();
        geometryStemPath = geometryStemPath.stem();

        string shaderKey = absolute(vertexPath).string() + "|" + absolute(fragmentPath).string();
        if (!geometryPath.empty())
        {
            shaderKey += "|" + absolute(geometryPath).string();
        }

        auto it = shaders.find(shaderKey);
        if (it != shaders.end())
        {
            shader.ID = it->second;
            return shader;
        }

        vector<string> vertSplit = String::Split(vertexPath, '/');
        vector<string> fragSplit = String::Split(fragmentPath, '/');

        string vertexCode;
        string fragmentCode;
        string geometryCode;
        ifstream vShaderFile;
        ifstream fShaderFile;
        ifstream gShaderFile;
        //ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            //open files
            vShaderFile.open(absolute(vertexPath).string());
            fShaderFile.open(absolute(fragmentPath).string());
            if (!geometryPath.empty())
            {
                gShaderFile.open(absolute(geometryPath).string());
            }

            if (!vShaderFile.is_open())
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Shader error: \nVertex: " + absolute(vertexPath).string() + "\n\n");
                return shader;
            }
            if (!fShaderFile.is_open())
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Shader error: \nFragment: " + absolute(fragmentPath).string() + "\n\n");
                return shader;
            }
            if (!geometryPath.empty()
                && !gShaderFile.is_open())
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Shader error: \nGeometry: " + absolute(geometryPath).string() + "\n\n");
                return shader;
            }

            if (geometryPath.empty())
            {
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
            else
            {
                stringstream 
                    vShaderStream, 
                    fShaderStream,
                    gShaderStream;
                //read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                gShaderStream << gShaderFile.rdbuf();
                //close file handlers
                vShaderFile.close();
                fShaderFile.close();
                gShaderFile.close();
                //convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
                geometryCode = gShaderStream.str();
            }
        }
        catch (const ios_base::failure& e)
        {
            if (geometryPath.empty())
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Error: File not successfully read: " +
                    to_string(e.code().value()) +
                    "\nVertex: " + absolute(vertexPath).string() +
                    "\nFragment: " + absolute(fragmentPath).string() + "\n\n");
            }
            else
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Error: File not successfully read: " +
                    to_string(e.code().value()) +
                    "\nVertex: " + absolute(vertexPath).string() +
                    "\nFragment: " + absolute(fragmentPath).string() +
                    "\nGeometry: " + absolute(geometryPath).string() + "\n\n");
            }
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geometryCode.c_str();

        unsigned int vertex{};
        unsigned int fragment{};
        unsigned int geometry{};

        //vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        if (!shader.CheckCompileErrors(
            vertexPath,
            fragmentPath,
            geometryPath,
            vertex, 
            "VERTEX"))
        {
            shader.ID = 0;
            return shader;
        }

        //fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        if (!shader.CheckCompileErrors(
            vertexPath,
            fragmentPath,
            geometryPath,
            fragment, 
            "FRAGMENT"))
        {
            shader.ID = 0;
            return shader;
        }

        if (geometryPath.empty())
        {
            //shader program
            shader.ID = glCreateProgram();
            glAttachShader(shader.ID, vertex);
            glAttachShader(shader.ID, fragment);
            glLinkProgram(shader.ID);
            if (!shader.CheckCompileErrors(
                vertexPath,
                fragmentPath,
                geometryPath,
                shader.ID, 
                "PROGRAM"))
            {
                shader.ID = 0;
                return shader;
            }
        }
        else
        {
            //geometry shader
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            if (!shader.CheckCompileErrors(
                vertexPath,
                fragmentPath,
                geometryPath,
                geometry, 
                "GEOMETRY"))
            {
                shader.ID = 0;
                return shader;
            }

            //shader program
            shader.ID = glCreateProgram();
            glAttachShader(shader.ID, vertex);
            glAttachShader(shader.ID, fragment);
            glAttachShader(shader.ID, geometry);
            glLinkProgram(shader.ID);
            if (!shader.CheckCompileErrors(
                vertexPath,
                fragmentPath,
                geometryPath,
                shader.ID, 
                "PROGRAM"))
            {
                shader.ID = 0;
                return shader;
            }
        }

        //delete shaders as they are no longer needed
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (!geometryPath.empty())
        {
            glDeleteShader(geometry);
        }

        shaders.emplace(shaderKey, shader.ID);

        string vertStem = path(vertexPath).filename().string();
        string fragStem = path(fragmentPath).filename().string();
        string geometryStem = path(geometryPath).filename().string();
        string message = geometryPath.empty()
            ? "Successfully loaded shader with vertex '" + vertStem
                + "' and fragment '" + fragStem + "'!\n"
            : "Successfully loaded shader with vertex '" + vertStem
                + "', fragment '" + fragStem
                + "' and geometry '" + geometryStem + "'!\n";
        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            message);

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

    bool Shader::CheckCompileErrors(
        string vertexPath,
        string fragmentPath,
        string geometryPath,
        GLuint shader, 
        const string& type)
    {
        string vertStem = path(vertexPath).filename().string();
        string fragStem = path(fragmentPath).filename().string();
        string geomStem = path(geometryPath).filename().string();

        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);

                string message = "Shader compilation error (" + type + "): " + infoLog
                    + "====================\n\n"
                    + "vertex shader: " + vertStem + "\n"
                    + "fragment shader: " + fragStem + "\n"
                    + "geometry shader: " + geomStem;

                Engine::CreateErrorPopup(message.c_str());

                return false;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);

                string message = "Shader linking error (" + type + "): " + infoLog 
                    + "====================\n\n" 
                    + "vertex shader: " + vertStem + "\n"
                    + "fragment shader: " + fragStem + "\n"
                    + "geometry shader: " + geomStem;

                Engine::CreateErrorPopup(message.c_str());

                return false;
            }
        }
        return true;
    }
}
