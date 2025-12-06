//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <array>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kg_registry.hpp"
#include "opengl/kg_opengl_shader.hpp"

namespace KalaGraphics::OpenGL::GameObject
{
	using std::vector;
	using std::array;
	using std::string;
	
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::Transform3D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
	using KalaHeaders::KalaMath::addpos;
	using KalaHeaders::KalaMath::setpos;
	using KalaHeaders::KalaMath::getpos;
	using KalaHeaders::KalaMath::addrot;
	using KalaHeaders::KalaMath::setrot;
	using KalaHeaders::KalaMath::getroteuler;
	using KalaHeaders::KalaMath::getrotquat;
	using KalaHeaders::KalaMath::getdirfront;
	using KalaHeaders::KalaMath::getdirright;
	using KalaHeaders::KalaMath::getdirup;
	
	using KalaGraphics::Core::KalaGraphicsRegistry;
	using KalaGraphics::OpenGL::OpenGL_Shader;
	
	//Light settings for this light source
	struct LIB_API OpenGL_PointLight_Data
	{
		//how bright is this point light, defaults to 1
		f32 intensity = 1.0f;
		//how far can this point light render, defaults to 50
		f32 maxRange = 50.0f;
		//the color of this point light, defaults to white
		vec3 color = 1.0f;
		
		//can this point light cast shadows, defaults to false
		bool canCastShadows{};
		//what is the current chosen shadow resolution, defaults to 1024x1024
		vec2 shadowResolution = 1024.0f;
		//how strong is the shadow, defaults to 1.0f
		f32 shadowStrength = 1.0f;
		//determines how large the sampling kernel for shadow sharpness is,
		//small radius - sharper shadows, large radius - softer shadows
		f32 filterRadius = 1.5f;
		//applies a small offset to the shadow map depth comparison
		//to prevent "grid" shadows
		f32 bias = 0.005f;
		//same as bias, but for angled non-flat surfaces
		f32 slopeBias = 0.01f;
		//minimum shadow render distance
		f32 nearPlane = 0.1f;
		//maximum shadow render distance, should be same as max range
		f32 farPlane = 50.0f;
		//prevents the denominator from becoming 0 when distance is 0,
		//usually 1 and never changed
		f32 constant = 1.0f;
		//controls linear falloff with distance,
		//high linear - light dies quickly, low linear - light reaches farther,
		//usually calculated from distance
		f32 linear = 0.09f;
		//controls exponential falloff (inverse square law),
		//usually calculated from distance
		f32 quadratic = 0.032f;
	};
	
	//Debug renderer settings for this light source
	struct LIB_API OpenGL_PointLight_Render
	{
		bool canUpdate = true;
		
		vec3 color = vec3(1.0f);
		f32 opacity = 1.0f;
		
		u32 VAO{};
		u32 VBO{};

		vector<vec3> vertices{};
		vector<u32> indices{};
		
		OpenGL_Shader* shader{};
	};
	
	class LIB_API OpenGL_PointLight
	{
	public:
		static inline KalaGraphicsRegistry<OpenGL_PointLight> registry{};
		
		static constexpr array<vec2, 5> availableShadowResolutions
		{
			128.0f,
			256.0f,
			512.0f,
			1024.0f,
			2048.0f
		};
		
		//
		// CORE
		//
		
		//Create a new point light
		static OpenGL_PointLight* Initialize(
			const string& name,
			u32 glID,
			OpenGL_Shader* shader);
			
		//Render this model. Requires handle (HDC) from your window
		bool Render(
			uintptr_t handle,
			const mat4& view,
			const mat4& projection);

		inline u32 GetID() const { return ID; }
		inline u32 GetGLID() const { return glID; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty, same as existing or too long
			if (newName.empty()
				|| newName == name
				|| newName.length() > 50) return;

			name = newName;
		}
		inline const string& GetName() { return name; }
		
		inline const vector<vec3>& GetVertices() const { return render.vertices; }
		inline const vector<u32>& GetIndices() const { return render.indices; }
			
		//
		// TRANSFORM
		//
		
		inline vec3 GetFront() { return getdirfront(transform); }
		inline vec3 GetRight() { return getdirright(transform); }
		inline vec3 GetUp() { return getdirup(transform); }

		//Increments position over time
		inline void AddPos(
			PosTarget type,
			const vec3& deltaPos)
		{
			addpos(
				transform,
				{},
				type,
				deltaPos);
		}
		//Snaps to given position
		inline void SetPos(
			PosTarget type,
			const vec3& newPos)
		{
			setpos(
				transform,
				{},
				type,
				newPos);
		}
		inline vec3 GetPos(PosTarget type) 
		{ 
			return getpos(
				transform,
				type); 
		}

		//Increments rotation over time
		inline void AddRot(
			RotTarget type,
			const vec3& deltaRot)
		{
			addrot(
				transform,
				{},
				type,
				deltaRot);
		}
		//Snaps to given rotation
		inline void SetRot(
			RotTarget type,
			const vec3& newRot)
		{
			setrot(
				transform,
				{},
				type,
				newRot);
		}
		inline vec3 GetRot(RotTarget type) 
		{ 
			return getroteuler(
				transform,
				type);
		}
		inline quat GetRotQuat(RotTarget type) 
		{ 
			return getrotquat(
				transform,
				type);
		}
		
		//
		// GRAPHICS
		//

		inline void SetNormalizedDebugColor(const vec3& newValue)
		{
			f32 clampX = clamp(newValue.x, 0.0f, 1.0f);
			f32 clampY = clamp(newValue.y, 0.0f, 1.0f);
			f32 clampZ = clamp(newValue.z, 0.0f, 1.0f);

			render.color = vec3(clampX, clampY, clampZ);
		}
		inline void SetDebugRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			f32 normalizedX = static_cast<f32>(clampX) / 255;
			f32 normalizedY = static_cast<f32>(clampY) / 255;
			f32 normalizedZ = static_cast<f32>(clampZ) / 255;

			render.color = vec3(normalizedX, normalizedY, normalizedZ);
		}

		inline const vec3& GetNormalizedDebugColor() const { return render.color; }
		inline vec3 GetDebugRGBColor() const
		{
			int rgbX = static_cast<int>(render.color.x * 255);
			int rgbY = static_cast<int>(render.color.y * 255);
			int rgbZ = static_cast<int>(render.color.z * 255);

			return vec3(rgbX, rgbY, rgbZ);
		}

		inline void SetOpacity(f32 newValue)
		{
			f32 clamped = clamp(newValue, 0.0f, 1.0f);
			render.opacity = clamped;
		}
		inline f32 GetOpacity() const { return render.opacity; }

		inline u32 GetVAO() const { return render.VAO; }
		inline u32 GetVBO() const { return render.VBO; }
		
		inline const OpenGL_Shader* GetShader() const { return render.shader; }
		
		~OpenGL_PointLight();
	private:
		bool isInitialized{};

		string name{};
		
		u32 ID{};
		u32 glID{};
		
		OpenGL_PointLight_Data data{};
		OpenGL_PointLight_Render render{};
		Transform3D transform{};
	};
}