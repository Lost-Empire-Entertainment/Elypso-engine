#pragma once

#ifndef GRAPHICS_SHADER_H
#define GRAPHICS_SHADER_H

//external
#include "glad.h"

#include <string>

namespace Graphics
{
	class Shader
	{
	public:
		static inline unsigned int ID;

		Shader(const std::string& vertexPath, const std::string& fragmentPath);

		void Use();

		void SetBool(const std::string &name, bool value) const;
		void SetInt(const std::string &name, int value) const;
		void SetFloat(const std::string &name, float value) const;

	private:
		void CheckCompileErrors(unsigned int shader, std::string type);
	};
}

#endif