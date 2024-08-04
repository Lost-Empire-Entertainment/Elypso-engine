//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <map>
#include <filesystem>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "render.hpp"
#include "modelchild.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "console.hpp"
#include "selectobject.hpp"
#include "gui_floatingdebugmenu.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::map;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;

using Graphics::Render;
using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using MeshType = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Physics::Select;
using Graphics::GUI::GUIFloatingDebugMenu;

namespace Graphics::Shape
{
	shared_ptr<GameObject> ModelChild::Initialize(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
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
		unsigned int& id)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

		GLuint VAO, VBO, EBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AssimpVertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)0);
		//vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, normal));
		//vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, texCoords));
		//vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, tangent));
		//vertex bitangent

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, bitangent));
		//ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, boneIDs));

		//weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, weights));
		glBindVertexArray(0);

		shared_ptr<Mesh> mesh = make_shared<Mesh>(MeshType::model, VAO, VBO, EBO);

		Shader modelChildShader = Shader::LoadShader(vertShader, fragShader);

		shared_ptr<Material> mat = make_shared<Material>();
		mat->AddShader(vertShader, fragShader, modelChildShader);

		shared_ptr<BasicShape_Variables> basicShape = make_shared<BasicShape_Variables>(shininess);

		if (name == tempName) name = "Model child";
		if (id == tempID) id = GameObject::nextID++;
		shared_ptr<GameObject> obj = make_shared<GameObject>(
			true,
			name,
			id,
			transform,
			mesh,
			mat,
			basicShape);

		obj->GetMesh()->SetVertices(vertices);
		obj->GetMesh()->SetIndices(indices);

		Texture::LoadTexture(obj, diffTexture, Material::TextureType::diffuse, false);
		Texture::LoadTexture(obj, specTexture, Material::TextureType::specular, false);
		Texture::LoadTexture(obj, "EMPTY", Material::TextureType::height, false);
		Texture::LoadTexture(obj, "EMPTY", Material::TextureType::normal, false);

		Shader assignedShader = obj->GetMaterial()->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);
		assignedShader.SetInt("material.specular", 1);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);

		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		GUIFloatingDebugMenu::UpdateCounts();

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void ModelChild::Render(
		const shared_ptr<GameObject>& obj,
		const mat4& view,
		const mat4& projection)
	{
		Shader shader = obj->GetMaterial()->GetShader();

		shader.Use();
		shader.SetVec3("viewPos", Render::camera.GetCameraPosition());
		shader.SetFloat("material.shininess", obj->GetBasicShape()->GetShininess());

		//directional light
		shader.SetVec3("dirLight.direction", Render::directionalDirection);
		shader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("dirLight.diffuse", Render::directionalDiffuse);
		shader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		shader.SetFloat("dirLight.intensity", Render::directionalIntensity);

		//point lights
		const vector<shared_ptr<GameObject>>& pointLights = GameObjectManager::GetPointLights();
		int pointLightCount = static_cast<int>(pointLights.size());
		shader.SetInt("pointLightCount", pointLightCount);
		if (pointLightCount > 0)
		{
			for (int i = 0; i < pointLightCount; i++)
			{
				shared_ptr<Transform> transform = pointLights[i]->GetTransform();
				shared_ptr<PointLight_Variables> pointLight = pointLights[i]->GetPointLight();
				string lightPrefix = "pointLights[" + to_string(i) + "].";
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

		//spotLight
		const vector<shared_ptr<GameObject>>& spotLights = GameObjectManager::GetSpotLights();
		int spotLightCount = static_cast<int>(spotLights.size());
		shader.SetInt("spotLightCount", spotLightCount);
		if (spotLightCount > 0)
		{
			for (int i = 0; i < spotLightCount; i++)
			{
				shared_ptr<Transform> transform = spotLights[i]->GetTransform();
				shared_ptr<SpotLight_Variables> spotLight = spotLights[i]->GetSpotLight();
				string lightPrefix = "spotLights[" + to_string(i) + "].";
				shader.SetVec3(lightPrefix + "position", transform->GetPosition());

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

		shared_ptr<Material> mat = obj->GetMaterial();

		//bind diffuse texture
		unsigned int diffuseTextureID = mat->GetTextureID(Material::TextureType::diffuse);
		if (diffuseTextureID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTextureID);
		}

		//bind specular texture
		unsigned int specularTextureID = mat->GetTextureID(Material::TextureType::specular);
		if (specularTextureID != 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularTextureID);
		}

		shader.SetMat4("model", model);

		GLuint VAO = obj->GetMesh()->GetVAO();
		glBindVertexArray(VAO);
		glDrawElements(
			GL_TRIANGLES,
			static_cast<unsigned int>(obj->GetMesh()->GetIndices().size()),
			GL_UNSIGNED_INT,
			0);

		glActiveTexture(GL_TEXTURE0);
	}
}