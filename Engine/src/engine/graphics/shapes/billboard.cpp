//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "billboard.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "meshcomponent.hpp"

using std::cout;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;

using Graphics::Shader;
using Graphics::Texture;
using Graphics::Components::Mesh;
using Type = Graphics::Components::Mesh::MeshType;
using Graphics::Components::Material;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Graphics::Render;
using Core::Select;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Billboard::InitializeBillboard(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& vertShader,
		const string& fragShader,
		const string& diffTexture,
		const float& shininess,
		string& name,
		unsigned int& id,
		const bool& isEnabled)
	{
		auto obj = GameObject::Create(
			name,
			id,
			isEnabled);

		float vertices[] =
		{
			//positions             //normals            //texture coords
			-0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
		};

		auto transform = make_shared<Transform>(pos, rot, scale);
		obj->SetTransform(transform);

		auto mesh = obj->AddComponent<Mesh>(true, Mesh::MeshType::billboard);
		mesh->Initialize(Mesh::MeshType::border, vertices, sizeof(vertices));

		auto material = obj->AddComponent<Material>();
		material->Initialize(
			(path(Engine::filesPath) / "shaders" / vertShader).string(),
			(path(Engine::filesPath) / "shaders" / fragShader).string());

		Shader billboardShader = material->GetShader();
		billboardShader.Use();
		billboardShader.SetInt("material.diffuse", 0);

		//load billboard texture
		Texture::LoadTexture(obj, diffTexture, Material::TextureType::diffuse, true);

		GameObjectManager::AddGameObject(obj);

		return obj;
	}

	void Billboard::RenderBillboard(
		const shared_ptr<GameObject>& obj, 
		const mat4& view, 
		const mat4& projection)
	{
		if (GameObjectManager::renderBillboards
			&& obj->IsEnabled())
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
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			shader.SetFloat("transparency", 1.0f);
			shader.SetVec3("color", vec3(1));

			mat4 model = mat4(1.0f);

			vec3 pos = obj->GetParent()->GetTransform()->GetPosition();
			obj->GetTransform()->SetPosition(pos);

			vec3 objectPos = obj->GetTransform()->GetPosition();
			vec3 cameraPos = Render::camera.GetCameraPosition();
			model = translate(model, objectPos);

			mat4 rotationMatrix = lookAt(objectPos, cameraPos, vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = inverse(rotationMatrix);
			model = rotationMatrix;

			model = scale(model, obj->GetTransform()->GetScale());

			//bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material->GetTextureID(Material::TextureType::diffuse));

			shader.SetMat4("model", model);
			GLuint VAO = mesh->GetVAO();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}