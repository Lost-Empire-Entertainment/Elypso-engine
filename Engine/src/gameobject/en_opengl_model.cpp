//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <algorithm>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/import_kmd.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kw_core.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"

#include "gameobject/en_opengl_model.hpp"
#include "gameobject/en_opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaMath::isnear;
using KalaHeaders::KalaMath::kclamp;
using KalaHeaders::KalaModelData::ModelHeader;
using KalaHeaders::KalaModelData::ModelTable;
using KalaHeaders::KalaModelData::ModelBlock;
using KalaHeaders::KalaModelData::Vertex;
using KalaHeaders::KalaModelData::ImportResult;
using KalaHeaders::KalaModelData::ResultToString;

using KalaWindow::Core::KalaWindowCore;
using namespace KalaWindow::OpenGL::OpenGLFunctions;
using KalaWindow::OpenGL::OpenGL_Global;

using ElypsoEngine::GameObject::OpenGL_Model;
using ElypsoEngine::GameObject::OpenGL_PointLight;
using ElypsoEngine::GameObject::OpenGL_PointLight_Data;
using ElypsoEngine::GameObject::MAX_PL_COUNT;
using ElypsoEngine::Graphics::TextureFormat;

using std::string;
using std::to_string;
using std::vector;
using std::make_unique;
using std::unique_ptr;
using std::filesystem::path;
using std::clamp;
	
static void CreateModelGeometry(
	const vector<Vertex>& vertices,
	const vector<u32>& indices,
	u32& outVAO,
	u32& outVBO,
	u32& outEBO);

namespace ElypsoEngine::GameObject
{	
	OpenGL_Model* OpenGL_Model::InitializeSingle(
		const string& name,
		OpenGL_Context* context,
		const vector<Vertex>& vertices,
		const vector<u32>& indices,
		OpenGL_Shader* shader)
	{
		if (!OpenGL_Global::IsContextValid(context))
		{
			Log::Print(
				"Cannot load model '" + name + "' because the gl context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}
		
		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to load model '" + name + "' because the shader context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return {};
		}
		
		return Initialize(
			name,
			vertices,
			indices,
			context,
			shader);
	}
		
	vector<OpenGL_Model*> OpenGL_Model::InitializeAll(
		const string& modelPath,
		OpenGL_Context* context,
		OpenGL_Shader* shader)
	{
		if (!OpenGL_Global::IsContextValid(context))
		{
			Log::Print(
				"Cannot load models from path '" + modelPath + "' because the gl context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return {};
		}
		
		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to load models from path '" + modelPath + "' because the shader context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return {};
		}
		
		ModelHeader header{};
		vector<ModelTable> tables{};
		vector<ModelBlock> blocks{};
		
		ImportResult result = ImportKMD(
			path(modelPath),
			header,
			tables,
			blocks);
			
		if (result != ImportResult::RESULT_SUCCESS)
		{
			Log::Print(
				"Failed to import model from path '" + modelPath + "'! Reason: " + ResultToString(result),
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);
			
			return {};
		}
		
		vector<OpenGL_Model*> models{};
		
		for (auto& b : blocks)
		{
			string nodeName = string(b.nodeName);
			vector<Vertex> vertices = b.vertices;
			vector<u32> indices = b.indices;
			
			OpenGL_Model* result = Initialize(
				move(nodeName),
				move(vertices),
				move(indices),
				context,
				shader);
				
			models.push_back(result);
		}
		
		return models;
	}
		
	vector<OpenGL_Model*> OpenGL_Model::StreamModels(
		const string& modelPath,
		const vector<ModelTable>& modelTables,
		OpenGL_Context* context,
		OpenGL_Shader* shader)
	{
		if (!OpenGL_Global::IsContextValid(context))
		{
			Log::Print(
				"Cannot stream models from path '" + modelPath + "' because the gl context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return {};
		}
		
		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to stream models from path '" + modelPath + "' because the shader context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return {};
		}
		
		return {};
	}
	
	OpenGL_Model* OpenGL_Model::Initialize(
		string name,
		vector<Vertex> vertices,
		vector<u32> indices,
		OpenGL_Context* context,
		OpenGL_Shader* shader)
	{
		u32 newID = ++KalaWindowCore::globalID;
		unique_ptr<OpenGL_Model> newModel = make_unique<OpenGL_Model>();
		OpenGL_Model* modelPtr = newModel.get();
		
		Log::Print(
			"Loading model '" + name + "' with ID '" + to_string(newID) + "'.",
			"OPENGL_MODEL",
			LogType::LOG_DEBUG);
			
		modelPtr->render.shader = shader;
		
		modelPtr->render.vertices = move(vertices);
		modelPtr->render.indices = move(indices);
		
		CreateModelGeometry(
			modelPtr->render.vertices,
			modelPtr->render.indices,
			modelPtr->render.VAO,
			modelPtr->render.VBO,
			modelPtr->render.EBO);

		//always called, ignored internally if ubo is already assigned
		InitializePointLightUBO(modelPtr->render.shader);
			
		modelPtr->ID = newID;
		modelPtr->context = context;
		modelPtr->SetName(name);
		
		modelPtr->isInitialized = true;
		
		registry.AddContent(newID, move(newModel));
		
		Log::Print(
			"Loaded model '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_MODEL",
			LogType::LOG_SUCCESS);

		return modelPtr;
	}
	
	bool OpenGL_Model::Render(
		const vec3& activeCameraPos,
		const mat4& view,
		const mat4& projection)
	{
		if (!render.canUpdate) return false;
		
		if (!OpenGL_Global::IsContextValid(context))
		{
			Log::Print(
                "Failed to render model '" + name + "' because its gl context is invalid!",
                "OPENGL_MODEL",
                LogType::LOG_ERROR,
                2);

            return false;
		}
		
		if (!render.shader)
		{
			Log::Print(
				"Failed to render model '" + name + "' because its shader is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (!render.shader->Bind())
		{
			Log::Print(
				"Failed to render model '" + name + "' because its shader '" + render.shader->GetName() + "' failed to bind!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		vec3 pos = GetPos(PosTarget::POS_COMBINED);
		quat rot = GetRotQuat(RotTarget::ROT_COMBINED);
		vec3 size = GetSize(SizeTarget::SIZE_COMBINED);

		mat4 model = createumodel(pos, rot, size);

		render.shader->SetMat4("uModel", model);
		render.shader->SetMat4("uView", view);
		render.shader->SetMat4("uProjection", projection);
		
		render.shader->SetVec3("uViewPos", activeCameraPos);

		bool isOpaque = isnear(render.opacity, 1.0f);
		bool isTransparentDiffuseTex = 
			render.diffuseTex
			&& render.diffuseTex->GetFormat() == TextureFormat::Format_RGBA8;

		bool isAlpha = 
			!isOpaque 
			|| isTransparentDiffuseTex;

		if (isAlpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}

		//diffuse data
		
		render.shader->SetVec3("uDiffuseColor", kclamp(render.diffuseColor, 0.0f, 1.0f));
		if (render.diffuseTex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, render.diffuseTex->GetTextureID());
			render.shader->SetInt("uDiffuseTex", 0);
			render.shader->SetBool("uHasDiffuseTex", true);
		}
		else render.shader->SetBool("uHasDiffuseTex", false);
		
		//normal data
		
		if (render.normalTex)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, render.normalTex->GetTextureID());
			render.shader->SetInt("uNormalTex", 1);
			render.shader->SetBool("uHasNormalTex", true);
		}
		else render.shader->SetBool("uHasNormalTex", false);
		
		//specular data
		
		render.shader->SetVec3("uSpecularColor", kclamp(render.specularColor, 0.0f, 1.0f));
		if (render.specularTex)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, render.specularTex->GetTextureID());
			render.shader->SetInt("uSpecularTex", 2);
			render.shader->SetBool("uHasSpecularTex", true);
		}
		else render.shader->SetBool("uHasSpecularTex", false);
		
		//emissive data
		
		render.shader->SetVec3( "uEmissiveColor", kclamp(render.emissiveColor, 0.0f, 1.0f));
		if (render.emissiveTex)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, render.emissiveTex->GetTextureID());
			render.shader->SetInt("uEmissiveTex", 3);
			render.shader->SetBool("uHasEmissiveTex", true);
		}
		else render.shader->SetBool("uHasEmissiveTex", false);

		//other data
		render.shader->SetFloat("uOpacity",   clamp(render.opacity, 0.0f, 1.0f));
		render.shader->SetFloat("uShininess", clamp(render.shininess, 1.0f, 64.0f));
		render.shader->SetBool("uTwoSided",   render.twoSided);
		
		u8 plCount = (u8)OpenGL_PointLight::registry.runtimeContent.size();
		
		if (plUBO != 0)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, plUBO);

			for (u8 i = 0; i < plCount; i++)
			{
				OpenGL_PointLight* pl = OpenGL_PointLight::registry.runtimeContent[i];

				//skip invalid lights
				if (!pl
					|| !pl->CanRenderLight()
					|| isnear(pl->GetIntensity())
					|| isnear(pl->GetMaxRange()))
				{
					OpenGL_PointLight_Data data{};
					data.canRender = 0;

					glBufferSubData(
						GL_UNIFORM_BUFFER,
						sizeof(OpenGL_PointLight_Data) * i,
						sizeof(OpenGL_PointLight_Data),
						&data);

					continue;
				}

				glBufferSubData(
					GL_UNIFORM_BUFFER,
					sizeof(OpenGL_PointLight_Data) * i,
					sizeof(OpenGL_PointLight_Data),
					pl->GetDataPtr());
			}

			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			render.shader->SetInt("uPointLightCount", plCount);
		}
		else render.shader->SetInt("uPointLightCount", 0);

		glBindVertexArray(render.VAO);
		glDrawElements(
			GL_TRIANGLES,
			render.indices.size(),
			GL_UNSIGNED_INT,
			0);
		glBindVertexArray(0);

		if (isAlpha)
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

		return true;
	}
	
	OpenGL_Model::~OpenGL_Model()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot destroy model '" + name + "' with ID '" + to_string(ID) + "' because it is not initialized!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying model '" + name + "' with ID '" + to_string(ID) + "'.",
			"OPENGL_MODEL",
			LogType::LOG_INFO);

		if (render.VAO != 0)
		{
			glDeleteVertexArrays(1, &render.VAO);
			render.VAO = 0;
		}
		if (render.VBO != 0)
		{
			glDeleteBuffers(1, &render.VBO);
			render.VBO = 0;
		}
		if (render.EBO != 0)
		{
			glDeleteBuffers(1, &render.EBO);
			render.EBO = 0;
		}
	}

	void OpenGL_Model::InitializePointLightUBO(OpenGL_Shader* shader)
	{
		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to initialize point light UBO because the shader context is invalid!",
				"OPENGL_MODEL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (plUBO != 0) return; //skip redundant reassigns

		u32 programID = shader->GetProgramID();

		glGenBuffers(1, &plUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, plUBO);

		glBufferData(
			GL_UNIFORM_BUFFER,
			sizeof(OpenGL_PointLight_Data) * MAX_PL_COUNT,
			nullptr,
			GL_DYNAMIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GLuint blockIndex = glGetUniformBlockIndex(
			programID,
			"PointLightBlock");
		glUniformBlockBinding(
			programID,
			blockIndex,
			0);

		glBindBufferBase(
			GL_UNIFORM_BUFFER,
			0,
			plUBO);
	}
}

void CreateModelGeometry(
	const vector<Vertex>& vertices,
	const vector<u32>& indices,
	u32& outVAO,
	u32& outVBO,
	u32& outEBO)
{
	if (vertices.empty()
		|| indices.empty())
	{
		KalaWindowCore::ForceClose(
			"OpenGL model error",
			"Failed to create model geometry because vertices or indices were empty");
	}
	
	glGenVertexArrays(1, &outVAO);
	glGenBuffers(1, &outVBO);
	glGenBuffers(1, &outEBO);
	
	glBindVertexArray(outVAO);
	
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, outVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW);
		
	//EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(u32),
		indices.data(),
		GL_STATIC_DRAW);
		
	//position - layout 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, position));
		
	//normal - layout 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, normal));
		
	//texcoord - layout 2
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, texCoord));
		
	//tangent - layout 3
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, tangent));
		
	glBindVertexArray(0);
}