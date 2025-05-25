//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

//external
#include "glm.hpp"
#include "glad.h"

namespace Graphics
{
	using std::string;
	using std::unordered_map;
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::mat2;
	using glm::mat3;
	using glm::mat4;

	class Shader
	{
	public:
		unsigned int ID{};

		static Shader LoadShader(
			const string& vertexPath = "", 
			const string& fragmentPath = "",
			const string& geometryPath = "");

		void Use() const;

		void SetBool(const string& name, bool value) const;
		void SetInt(const string& name, int value) const;
		void SetFloat(const string& name, float value) const;

		void SetVec2(const string& name, const vec2& value) const;
		void SetVec2(const string& name, float x, float y) const;

		void SetVec3(const string& name, const vec3& value) const;
		void SetVec3(const string& name, float x, float y, float z) const;

		void SetVec4(const string& name, const vec4& value) const;
		void SetVec4(const string& name, float x, float y, float z, float w) const;

		void SetMat2(const string& name, const mat2& mat) const;
		void SetMat3(const string& name, const mat3& mat) const;
		void SetMat4(const string& name, const mat4& mat) const;
	private:
		static unordered_map<string, unsigned int> shaders;

		bool CheckCompileErrors(
			string vertexPath,
			string fragmentPath,
			string geometryPath,
			GLuint shader,
			const string& type);
	};
}