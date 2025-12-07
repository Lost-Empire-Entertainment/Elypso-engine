//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kmd.hpp"

#include "opengl/kg_opengl_shader.hpp"
#include "opengl/kg_opengl_texture.hpp"
#include "core/kg_registry.hpp"

namespace KalaGraphics::OpenGL::GameObject
{
	using std::vector;
	using std::string;
	
	using KalaHeaders::KalaMath::kclamp;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::toquat;
	using KalaHeaders::KalaMath::Transform3D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
	using KalaHeaders::KalaMath::SizeTarget;
	using KalaHeaders::KalaMath::addpos;
	using KalaHeaders::KalaMath::setpos;
	using KalaHeaders::KalaMath::getpos;
	using KalaHeaders::KalaMath::addrot;
	using KalaHeaders::KalaMath::setrot;
	using KalaHeaders::KalaMath::getroteuler;
	using KalaHeaders::KalaMath::getrotquat;
	using KalaHeaders::KalaMath::addsize;
	using KalaHeaders::KalaMath::setsize;
	using KalaHeaders::KalaMath::getsize;
	using KalaHeaders::KalaMath::getdirfront;
	using KalaHeaders::KalaMath::getdirright;
	using KalaHeaders::KalaMath::getdirup;
	using KalaHeaders::KalaModelData::ModelTable;
	using KalaHeaders::KalaModelData::Vertex;
	
	using KalaGraphics::OpenGL::OpenGL_Shader;
	using KalaGraphics::OpenGL::OpenGL_Texture;
	using KalaGraphics::Core::KalaGraphicsRegistry;
	
	struct LIB_API OpenGL_Model_Render
	{
		bool canUpdate = true;
		
		//the transparency of this model
		f32 opacity = 1.0f;
		//the reflectiveness of this model
		f32 shininess = 32.0f;
		//can this model render on both sides of each face
		bool twoSided{};
		
		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		vector<Vertex> vertices{};
		vector<u32> indices{};
		
		OpenGL_Shader* shader{};
		
		OpenGL_Texture* diffuseTex{};
		vec3 diffuseColor = 1.0f;
		
		OpenGL_Texture* normalTex{};
		
		OpenGL_Texture* specularTex{};
		vec3 specularColor = 1.0f;
		
		OpenGL_Texture* emissiveTex{};
		vec3 emissiveColor = {};
	};
	
	class LIB_API OpenGL_Model
	{
	public:
		static inline KalaGraphicsRegistry<OpenGL_Model> registry{};
		
		//get global point light UBO
		static inline u32 GetPointLightUBO() { return plUBO; }
		
		//
		// CORE
		//
		
		//Create a single model from basic data
		static OpenGL_Model* InitializeSingle(
			const string& name,
			u32 glID,
			const vector<Vertex>& vertices,
			const vector<u32>& indices,
			OpenGL_Shader* shader);
		
		//Initialize all models from a .kmd file. Returns a vector of non-owning pointers
		//because a .kmd file may hold more than one model
		static vector<OpenGL_Model*> InitializeAll(
			const string& modelPath,
			u32 glID,
			OpenGL_Shader* shader);
		
		//Stream models based off of the provided tables
		static vector<OpenGL_Model*> StreamModels(
			const string& modelPath,
			const vector<ModelTable>& modelTables,
			u32 glID,
			OpenGL_Shader* shader);
		
		inline bool IsInitialized() const { return isInitialized; }
		
		///Render this model. Requires handle (HDC) from your window
		bool Render(
			const vec3& activeCameraPos,
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
		
		inline void SetUpdateState(bool newValue) { render.canUpdate = newValue; }
		inline bool CanUpdate() const { return render.canUpdate; }
		
		inline const vector<Vertex>& GetVertices() const { return render.vertices; }
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
		
		//Increments size over time
		inline void AddSize(
			SizeTarget type,
			const vec3& deltaSize)
		{
			addsize(
				transform,
				{},
				type,
				deltaSize);
		}
		//Snaps to given size
		inline void SetSize(
			SizeTarget type,
			const vec3& newSize)
		{
			setsize(
				transform,
				{},
				type,
				newSize);
		}
		inline vec3 GetSize(SizeTarget type) 
		{ 
			return getsize(
				transform,
				type); 
		}
		
		//
		// GRAPHICS
		//

		inline void SetNormalizedDiffuseColor(const vec3& newValue)
		{
			render.diffuseColor = kclamp(newValue, 0.0f, 1.0f);
		}
		inline void SetDiffuseRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			f32 normalizedX = static_cast<f32>(clampX) / 255;
			f32 normalizedY = static_cast<f32>(clampY) / 255;
			f32 normalizedZ = static_cast<f32>(clampZ) / 255;

			render.diffuseColor = vec3(normalizedX, normalizedY, normalizedZ);
		}

		inline const vec3& GetNormalizedDiffuseColor() const { return render.diffuseColor; }
		inline vec3 GetDiffuseRGBColor() const
		{
			int rgbX = static_cast<int>(render.diffuseColor.x * 255);
			int rgbY = static_cast<int>(render.diffuseColor.y * 255);
			int rgbZ = static_cast<int>(render.diffuseColor.z * 255);

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
		inline u32 GetEBO() const { return render.EBO; }

		inline const OpenGL_Shader* GetShader() const { return render.shader; }

		inline void SetDiffuseTexture(OpenGL_Texture* newTexture)
		{
			if (newTexture
				&& render.diffuseTex != newTexture)
			{
				render.diffuseTex = newTexture;
			}
		}
		inline void ClearDiffuseTexture() { render.diffuseTex = nullptr; }
		inline const OpenGL_Texture* GetDiffuseTexture() const { return render.diffuseTex; }
		
		~OpenGL_Model();
	private:
		//Initialize global point light UBO
		static void InitializePointLightUBO(OpenGL_Shader* shader);
	
		static OpenGL_Model* Initialize(
			string name,
			vector<Vertex> vertices,
			vector<u32> indices,
			u32 glID,
			OpenGL_Shader* shader);
			
		//point light UBO reused by all models
		static inline u32 plUBO{};
	
		bool isInitialized{};

		string name{};
		
		u32 ID{};
		u32 glID{};
		
		OpenGL_Model_Render render{};
		Transform3D transform{};
	};
}