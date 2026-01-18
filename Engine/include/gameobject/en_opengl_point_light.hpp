//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <array>
#include <string>

#include "KalaHeaders/math_utils.hpp"

#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_shader.hpp"

#include "core/en_registry.hpp"

namespace ElypsoEngine::GameObject
{
	using std::vector;
	using std::array;
	using std::string;
	
	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::vec4;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::Transform3D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
	using KalaHeaders::KalaMath::SizeTarget;
	
	using KalaWindow::OpenGL::OpenGL_Context;
	using KalaWindow::OpenGL::OpenGL_Shader;

	using ElypsoEngine::Core::EngineRegistry;
	
	constexpr u8 MAX_PL_COUNT = 128;
	
	//Light settings for this light source
	struct OpenGL_PointLight_Data
	{
		//combined pos from transform, w unused
		vec4 pos{};
		
		//canUpdate copy from OpenGL_PointLight_Render
		int canRender = 1;
		//how bright is this point light, defaults to 1
		f32 intensity = 1.0f;
		//how far can this point light render, defaults to 50
		f32 maxRange = 50.0f;
		f32 _pad1{};
		
		//the color of this point light, defaults to white, w unused
		vec4 color = 1.0f;
		
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
		f32 _pad2{};
		
		//shadow resolution in x and y, defaults to 1024x1024,
		//canCastShadows in z, w unused
		vec4 shadowResolution = vec4(1024.0f, 1024.0f, 0.0f, 0.0f);
		
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
		f32 _pad3[2];
	};
	
	//Debug renderer settings for this light source
	struct OpenGL_PointLight_Render
	{
		bool canUpdate = true;
		
		vec3 color = vec3(1.0f);
		f32 opacity = 1.0f;
		
		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		vector<vec3> vertices{};
		vector<u32> indices{};
		
		OpenGL_Shader* shader{};
	};

	static constexpr array<vec2, 5> availableShadowResolutions
	{
		128.0f,
		256.0f,
		512.0f,
		1024.0f,
		2048.0f
	};
	
	class OpenGL_PointLight
	{
	public:
		static EngineRegistry<OpenGL_PointLight>& GetRegistry();
		
		//
		// CORE
		//
		
		//Create a new point light
		static OpenGL_PointLight* Initialize(
			const string& name,
			OpenGL_Context* context,
			vector<vec3> vertices = {},
			vector<u32> indices = {},
			OpenGL_Shader* shader = {});

		bool IsInitialized() const;
			
		bool Render(
			const mat4& view,
			const mat4& projection);

		u32 GetID() const;

		OpenGL_Context* GetContext() const;

		void SetName(const string& newName);
		const string& GetName();
		
		void SetRenderDebugShapeState(bool newValue);
		bool CanRenderDebugShape() const;
		
		void SetRenderLightState(bool newValue);
		bool CanRenderLight() const;
		
		const vector<vec3>& GetVertices() const;
		const vector<u32>& GetIndices() const;
			
		//
		// TRANSFORM
		//
		
		vec3 GetFront();
		vec3 GetRight();
		vec3 GetUp();

		//Increments position over time
		void AddPos(
			PosTarget type,
			const vec3& deltaPos);
		//Snaps to given position
		void SetPos(
			PosTarget type,
			const vec3& newPos);
		vec3 GetPos(PosTarget type);

		//Increments rotation over time
		void AddRot(
			RotTarget type,
			const vec3& deltaRot);
		//Snaps to given rotation
		void SetRot(
			RotTarget type,
			const vec3& newRot);
		vec3 GetRot(RotTarget type);
		quat GetRotQuat(RotTarget type);
		
		//Increments size over time
		void AddSize(
			SizeTarget type,
			const vec3& deltaSize);
		//Snaps to given size
		void SetSize(
			SizeTarget type,
			const vec3& newSize);
		vec3 GetSize(SizeTarget type);
		
		//
		// GRAPHICS
		//

		void SetNormalizedDebugColor(const vec3& newValue);
		void SetDebugRGBColor(const vec3& newValue);

		const vec3& GetNormalizedDebugColor() const;
		vec3 GetDebugRGBColor() const;

		void SetOpacity(f32 newValue);
		f32 GetOpacity() const;

		u32 GetVAO() const;
		u32 GetVBO() const;
		u32 GetEBO() const;
		
		const OpenGL_Shader* GetShader() const;
		
		//
		// LIGHT DATA
		//
		
		void SetIntensity(f32 newValue);
		f32 GetIntensity() const;
		
		void SetMaxRange(f32 newValue);
		f32 GetMaxRange() const;
		
		void SetNormalizedColor(const vec3& newValue);
		void SetRGBColor(const vec3& newValue);

		vec3 GetNormalizedColor() const;
		vec3 GetRGBColor() const;
		
		void SetConstant(f32 newValue);
		f32 GetConstant() const;
		
		void SetLinear(f32 newValue);
		f32 GetLinear() const;
		
		void SetQuadratic(f32 newValue);
		f32 GetQuadratic() const;
		
		const OpenGL_PointLight_Data* GetDataPtr() const;
		
		~OpenGL_PointLight();
	private:
		bool isInitialized{};

		string name{};
		
		u32 ID{};

		OpenGL_Context* context{};
		
		OpenGL_PointLight_Data data{};
		OpenGL_PointLight_Render render{};
		Transform3D transform{};
	};
}