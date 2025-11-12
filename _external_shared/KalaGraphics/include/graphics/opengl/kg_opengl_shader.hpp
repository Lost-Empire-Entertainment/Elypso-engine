//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "utils/kg_registry.hpp"

namespace KalaGraphics::Graphics::OpenGL
{
	using std::string;
	using std::array;

	using KalaHeaders::vec2;
	using KalaHeaders::vec3;
	using KalaHeaders::vec4;
	using KalaHeaders::mat2;
	using KalaHeaders::mat3;
	using KalaHeaders::mat4;
	
	using KalaGraphics::Utils::KalaGraphicsRegistry;

	enum class ShaderType
	{
		SHADER_NONE,
		SHADER_VERTEX,
		SHADER_FRAGMENT,
		SHADER_GEOMETRY
	};

	struct LIB_API ShaderData
	{
		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderPath{};

		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderData{};

		//assign to something else than the default 'SHADER_NONE'
		ShaderType type = ShaderType::SHADER_NONE;

		//ID of the shader, does not need to be filled by user
		u32 ID{};
	};

	class LIB_API OpenGL_Shader
	{
	public:
		static inline KalaGraphicsRegistry<OpenGL_Shader> registry{};

		//Create a new shader with up to three types of shader files.
		//Geometry shaders are optional but vert and frag shader must always be filled
		static OpenGL_Shader* CreateShader(
			u32 glID,
			const string& shaderName,
			const array<ShaderData, 3>& shaderData);

		inline bool IsInitialized() const { return isInitialized; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//vertex, fragment and geometry shader compilation messages will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		inline const string& GetName() const { return name; }
		inline bool SetName(const string& newName)
		{
			if (newName.empty()
				|| newName.size() > 50)
			{
				return false;
			}

			name = newName;

			return true;
		}

		//Returns the global ID of this shader
		inline u32 GetID() const { return ID; }
		//Returns the OpenGL program ID of this shader
		inline u32 GetProgramID() const { return programID; }
		//Returns the OpenGL context ID of this shader
		inline u32 GetGLID() const { return glID; }

		//Returns true if this shader is loaded
		inline bool IsShaderLoaded(ShaderType targetType) const
		{
			if (programID == 0) return false;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.ID != 0;
			case ShaderType::SHADER_FRAGMENT: return fragData.ID != 0;
			case ShaderType::SHADER_GEOMETRY: return geomData.ID != 0;
			}

			return false;
		}

		//Returns shader data assigned to shader type
		inline const string& GetShaderData(ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.shaderData;
			case ShaderType::SHADER_FRAGMENT: return fragData.shaderData;
			case ShaderType::SHADER_GEOMETRY: return geomData.shaderData;
			}

			return empty;
		}
		//Returns shader path assigned to shader type
		inline const string& GetShaderPath(ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.shaderPath;
			case ShaderType::SHADER_FRAGMENT: return fragData.shaderPath;
			case ShaderType::SHADER_GEOMETRY: return geomData.shaderPath;
			}

			return empty;
		}
		inline u32 GetShaderID(ShaderType targetType) const
		{
			if (programID == 0) return 0;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.ID;
			case ShaderType::SHADER_FRAGMENT: return fragData.ID;
			case ShaderType::SHADER_GEOMETRY: return geomData.ID;
			}

			return 0;
		}

		inline const array<ShaderData, 3>& GetAllShaders() const
		{
			static array<ShaderData, 3> dataOut{};

			dataOut[0] = vertData;
			dataOut[1] = fragData;
			dataOut[2] = geomData;

			return dataOut;
		}

		//Bind current shader, requires handle (HDC) from your window
		bool Bind(
			u32 glID,
			uintptr_t handle);

		bool HotReload(u32 windowID);

		void SetBool(u32 programID, const string& name, bool value) const;
		void SetInt(u32 programID, const string& name, i32 value) const;
		void SetFloat(u32 programID, const string& name, f32 value) const;

		void SetVec2(u32 programID, const string& name, const vec2& value) const;
		void SetVec3(u32 programID, const string& name, const vec3& value) const;
		void SetVec4(u32 programID, const string& name, const vec4& value) const;

		void SetMat2(u32 programID, const string& name, const mat2& mat) const;
		void SetMat3(u32 programID, const string& name, const mat3& mat) const;
		void SetMat4(u32 programID, const string& name, const mat4& mat) const;

		//Do not destroy manually, erase from registry instead
		~OpenGL_Shader();
	private:
		static inline bool isVerboseLoggingEnabled{};

		bool isInitialized{};
		bool checkedBindOnce{};

		string name{};

		u32 ID{};
		u32 programID{};
		u32 glID{};

		ShaderData vertData{};
		ShaderData fragData{};
		ShaderData geomData{};
	};
}