//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kmd.hpp"

#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_shader.hpp"

#include "graphics/en_opengl_texture.hpp"
#include "core/en_registry.hpp"

namespace ElypsoEngine::GameObject
{
	using std::vector;
	using std::string;
	
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::Transform3D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
	using KalaHeaders::KalaMath::SizeTarget;

	using KalaHeaders::KalaModelData::ModelTable;
	using KalaHeaders::KalaModelData::Vertex;

	using KalaWindow::OpenGL::OpenGL_Context;
	using KalaWindow::OpenGL::OpenGL_Shader;

	using ElypsoEngine::Graphics::OpenGL_Texture;
	using ElypsoEngine::Core::EngineRegistry;
	
	struct OpenGL_Model_Render
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
	
	class OpenGL_Model
	{
	public:
		static EngineRegistry<OpenGL_Model>& GetRegistry();

		//get global point light UBO
		static u32 GetPointLightUBO();
		
		//
		// CORE
		//
		
		//Create a single model from basic data
		static OpenGL_Model* InitializeSingle(
			const string& name,
			OpenGL_Context* context,
			const vector<Vertex>& vertices,
			const vector<u32>& indices,
			OpenGL_Shader* shader);
		
		//Initialize all models from a .kmd file. Returns a vector of non-owning pointers
		//because a .kmd file may hold more than one model
		static vector<OpenGL_Model*> InitializeAll(
			const string& modelPath,
			OpenGL_Context* context,
			OpenGL_Shader* shader);
		
		//Stream models based off of the provided tables
		static vector<OpenGL_Model*> StreamModels(
			const string& modelPath,
			const vector<ModelTable>& modelTables,
			OpenGL_Context* context,
			OpenGL_Shader* shader);
		
		bool IsInitialized() const;
		
		bool Render(
			const vec3& activeCameraPos,
			const mat4& view,
			const mat4& projection);

		u32 GetID() const;

		OpenGL_Context* GetContext() const;

		void SetName(const string& newName);
		const string& GetName();
		
		void SetUpdateState(bool newValue);
		bool CanUpdate() const;
		
		const vector<Vertex>& GetVertices() const;
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

		void SetNormalizedDiffuseColor(const vec3& newValue);
		void SetDiffuseRGBColor(const vec3& newValue);

		const vec3& GetNormalizedDiffuseColor() const;
		vec3 GetDiffuseRGBColor() const;

		void SetOpacity(f32 newValue);
		f32 GetOpacity() const;

		u32 GetVAO() const;
		u32 GetVBO() const;
		u32 GetEBO() const;

		const OpenGL_Shader* GetShader() const;

		void SetDiffuseTexture(OpenGL_Texture* newTexture);
		void ClearDiffuseTexture();
		const OpenGL_Texture* GetDiffuseTexture() const;
		
		~OpenGL_Model();
	private:	
		static OpenGL_Model* Initialize(
			string name,
			vector<Vertex> vertices,
			vector<u32> indices,
			OpenGL_Context* context,
			OpenGL_Shader* shader);

		//Initialize global point light UBO
		static void InitializePointLightUBO(OpenGL_Shader* shader);
	
		bool isInitialized{};

		string name{};
		
		u32 ID{};

		OpenGL_Context* context{};
		
		OpenGL_Model_Render render{};
		Transform3D transform{};
	};
}