//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <map>
#include <fstream>
#include <string>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "render.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"
#include "gameObjectFile.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lighttcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using std::cout;
using std::endl;
using std::to_string;
using std::map;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::ofstream;
using std::ifstream;
using std::filesystem::exists;
using std::stoul;
using std::stof;

using Graphics::Render;
using Graphics::Shader;
using Graphics::Texture;
using Graphics::Components::Mesh;
using MeshType = Graphics::Components::Mesh::MeshType;
using Graphics::Components::Material;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Core::Select;
using EngineFile::GameObjectFile;
using Graphics::Components::PointLightComponent;
using Graphics::Components::SpotLightComponent;
using Graphics::Components::DirectionalLightComponent;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> Model::Initialize(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		const string& modelPath,
		const string& vertShader,
		const string& fragShader,
		const string& diffTexture,
		const string& specTexture,
		const string& normalTexture,
		const string& heightTexture,
		const vector<AssimpVertex> vertices,
		const vector<unsigned int> indices,
		const float& shininess,
		string& name,
		unsigned int& id,
		const bool& isEnabled,
		const bool& isMeshEnabled)
	{
		auto obj = GameObject::Create(
			name,
			id,
			isEnabled,
			pos,
			rot,
			scale);

		auto mesh = obj->AddComponent<Mesh>(true, Mesh::MeshType::billboard);
		mesh->InitializeModel(Mesh::MeshType::border, vertices);

		auto material = obj->AddComponent<Material>();
		material->Initialize(
			(path(Engine::filesPath) / "shaders" / vertShader).string(),
			(path(Engine::filesPath) / "shaders" / fragShader).string());

		mesh->SetVertices(vertices);
		mesh->SetIndices(indices);

		//load model textures
		Texture::LoadTexture(obj, diffTexture, Material::TextureType::diffuse, false);
		Texture::LoadTexture(obj, specTexture, Material::TextureType::specular, false);
		Texture::LoadTexture(obj, "EMPTY", Material::TextureType::height, false);
		Texture::LoadTexture(obj, "EMPTY", Material::TextureType::normal, false);

		Shader modelShader = material->GetShader();
		modelShader.Use();
		modelShader.SetInt("material.diffuse", 0);
		modelShader.SetInt("material.specular", 1);

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);

#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif
		GameObjectFile::LoadModel(txtFilePath);

		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void Model::Render(
		const shared_ptr<GameObject>& obj,
		const mat4& view,
		const mat4& projection)
	{
		if (obj->IsEnabled())
		{
			auto material = obj->GetComponent<Material>();
			auto mesh = obj->GetComponent<Mesh>();
			if (!material
				|| !mesh)
			{
				return;
			}

			Shader shader = material->GetShader();

			shader.Use();
			shader.SetVec3("viewPos", Render::camera.GetCameraPosition());
			shader.SetFloat("material.shininess", 32);

			//directional light
			if (GameObjectManager::GetDirectionalLight() != nullptr)
			{
				shared_ptr<GameObject> dirLight = GameObjectManager::GetDirectionalLight();

				int count = dirLight->IsEnabled() ? 1 : 0;
				shader.SetInt("dirLightCount", count);
				
				shared_ptr<Transform> transform = dirLight->GetTransform();
				auto dirVar = dirLight->GetComponent<DirectionalLightComponent>();

				vec3 dirRot = transform->GetRotation();
				quat dirQuat = quat(radians(dirRot));
				//assuming the initial direction is along the negative Y-axis
				vec3 initialDir = vec3(0.0f, -1.0f, 0.0f);
				//rotate the initial direction using the quaternion
				vec3 rotatedDir = dirQuat * initialDir;
				rotatedDir = normalize(rotatedDir);

				shader.SetBool("dirLight.enabled", dirLight->IsEnabled());

				shader.SetVec3("dirLight.direction", rotatedDir);

				shader.SetFloat("dirLight.intensity", dirVar->GetIntensity());
				shader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
				shader.SetVec3("dirLight.diffuse", dirVar->GetDiffuse());
				shader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			}
			else 
			{
				shader.SetBool("dirLight.enabled", false);
			}

			//point lights
			const vector<shared_ptr<GameObject>>& pointLights = GameObjectManager::GetPointlights();
			int pointLightCount = static_cast<int>(pointLights.size());
			shader.SetInt("pointLightCount", pointLightCount);
			if (pointLightCount > 0)
			{
				for (int i = 0; i < pointLightCount; i++)
				{
					shared_ptr<Transform> transform = pointLights[i]->GetTransform();
					auto pointLight = pointLights[i]->GetComponent<PointLightComponent>();
					string lightPrefix = "pointLights[" + to_string(i) + "].";

					shader.SetBool(lightPrefix + "enabled", pointLights[i]->IsEnabled());

					shader.SetVec3(lightPrefix + "position", transform->GetPosition());
					shader.SetVec3(lightPrefix + "ambient", 0.05f, 0.05f, 0.05f);
					shader.SetVec3(lightPrefix + "diffuse", pointLight->GetDiffuse());
					shader.SetVec3(lightPrefix + "specular", 1.0f, 1.0f, 1.0f);
					shader.SetFloat(lightPrefix + "constant", 1.0f);
					shader.SetFloat(lightPrefix + "linear", 0.09f);
					shader.SetFloat(lightPrefix + "quadratic", 0.032f);
					shader.SetFloat(lightPrefix + "intensity", pointLight->GetIntensity());
					shader.SetFloat(lightPrefix + "distance", pointLight->GetDistance());
				}
			}

			//spotlights
			const vector<shared_ptr<GameObject>>& spotLights = GameObjectManager::GetSpotLights();
			int spotLightCount = static_cast<int>(spotLights.size());
			shader.SetInt("spotLightCount", spotLightCount);
			if (spotLightCount > 0)
			{
				for (int i = 0; i < spotLightCount; i++)
				{
					shared_ptr<Transform> transform = spotLights[i]->GetTransform();
					auto spotLight = spotLights[i]->GetComponent<SpotLightComponent>();
					string lightPrefix = "spotLights[" + to_string(i) + "].";
					shader.SetVec3(lightPrefix + "position", transform->GetPosition());

					shader.SetBool(lightPrefix + "enabled", spotLights[i]->IsEnabled());

					vec3 rotationAngles = transform->GetRotation();
					quat rotationQuat = quat(radians(rotationAngles));
					//assuming the initial direction is along the negative Y-axis
					vec3 initialDirection = vec3(0.0f, -1.0f, 0.0f);
					//rotate the initial direction using the quaternion
					vec3 rotatedDirection = rotationQuat * initialDirection;
					//set the rotated direction in the shader
					shader.SetVec3(lightPrefix + "direction", rotatedDirection);

					shader.SetFloat(lightPrefix + "intensity", spotLight->GetIntensity());
					shader.SetFloat(lightPrefix + "distance", spotLight->GetDistance());
					shader.SetVec3(lightPrefix + "ambient", 0.0f, 0.0f, 0.0f);
					shader.SetVec3(lightPrefix + "diffuse", spotLight->GetDiffuse());
					shader.SetVec3(lightPrefix + "specular", 1.0f, 1.0f, 1.0f);
					shader.SetFloat(lightPrefix + "constant", 1.0f);
					shader.SetFloat(lightPrefix + "linear", 0.09f);
					shader.SetFloat(lightPrefix + "quadratic", 0.032f);
					shader.SetFloat(lightPrefix + "cutOff", cos(radians(spotLight->GetInnerAngle())));
					shader.SetFloat(lightPrefix + "outerCutOff", cos(radians(spotLight->GetOuterAngle())));
				}
			}

			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			mat4 model = mat4(1.0f);

			model = translate(model, obj->GetTransform()->GetPosition());

			quat newRot = quat(radians(obj->GetTransform()->GetRotation()));
			model *= mat4_cast(newRot);

			model = scale(model, obj->GetTransform()->GetScale());

			//bind diffuse texture
			unsigned int diffuseTextureID = material->GetTextureID(Material::TextureType::diffuse);
			if (diffuseTextureID != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTextureID);
			}

			//bind specular texture
			unsigned int specularTextureID = material->GetTextureID(Material::TextureType::specular);
			if (specularTextureID != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularTextureID);
			}

			shader.SetMat4("model", model);

			GLuint VAO = mesh->GetVAO();
			glBindVertexArray(VAO);
			glDrawElements(
				GL_TRIANGLES,
				static_cast<unsigned int>(mesh->GetIndices().size()),
				GL_UNSIGNED_INT,
				0);

			glActiveTexture(GL_TEXTURE0);
		}
	}
}