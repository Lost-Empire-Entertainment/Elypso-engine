//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#include "console.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;
using std::filesystem::exists;
using std::to_string;

using Graphics::Shader;
using Graphics::Texture;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using MeshType = Graphics::Components::MeshComponent::MeshType;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Graphics::Render;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Billboard::InitializeBillboard(
		bool isBillboardUI,
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& diffTexture,
		unsigned int& id,
		const bool& isEnabled)
	{
		if (id == tempID)
		{
			id = ++GameObject::nextID;
		}

		auto obj = make_shared<GameObject>("Billboard", id, isEnabled);
		auto transform = obj->AddComponent<TransformComponent>();
		transform->SetOwner(obj);
		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scale);
		obj->SetBillboardUI(isBillboardUI);

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

		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		auto mesh = obj->AddComponent<MeshComponent>(
			MeshType::billboard, 
			vao, 
			vbo, 
			ebo);
		mesh->SetOwner(obj);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for selected object border is invalid!");
		}

		Shader billboardShader = Shader::LoadShader(vert, frag);

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->SetOwner(obj);
		mat->AddShader(vert, frag, "", billboardShader);

		Texture::LoadTexture(obj, diffTexture, MaterialComponent::TextureType::diffuse, true);

		Shader assignedShader = mat->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddTransparentObject(obj);

		if (!isBillboardUI)
		{
			GameObjectManager::AddBillboard(obj);
		}
		else
		{
			GameObjectManager::AddBillboardUI(obj);
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized Billboard with name " + obj->GetName() + " and ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void Billboard::RenderBillboard(
		const shared_ptr<GameObject>& obj, 
		const mat4& view, 
		const mat4& projection)
	{
		if (obj == nullptr) Engine::CreateErrorPopup("Billboard gameobject is invalid.");
		if (obj->GetParentBillboardHolder() == nullptr) Engine::CreateErrorPopup("Billboard parent gameobject is invalid.");
		shared_ptr<GameObject> parent = obj->GetParentBillboardHolder();

		MeshComponent::MeshType type = parent->GetComponent<MeshComponent>()->GetMeshType();

		//checks whether this object type matches the billboard types that are rendered
		bool correctType =
			(type == MeshComponent::MeshType::point_light
			&& GameObjectManager::renderPointLightBillboards)
			|| (type == MeshComponent::MeshType::spot_light
			&& GameObjectManager::renderSpotlightBillboards)
			|| (type == MeshComponent::MeshType::directional_light
			&& GameObjectManager::renderDirLightBillboard)
			|| (type == MeshComponent::MeshType::audio
			&& GameObjectManager::renderAudioObjectBillboards)
			|| (type == MeshComponent::MeshType::camera
			&& GameObjectManager::renderAudioObjectBillboards);

		if (correctType
			&& obj->IsEnabled())
		{
			auto mat = obj->GetComponent<MaterialComponent>();

			Shader shader = mat->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			shader.SetFloat("transparency", 1.0f);
			shader.SetVec3("color", vec3(1));

			mat4 model = mat4(1.0f);

			vec3 pos = parent->GetComponent<TransformComponent>()->GetPosition();
			obj->GetComponent<TransformComponent>()->SetPosition(pos);

			vec3 objectPos = obj->GetComponent<TransformComponent>()->GetPosition();

			auto tc = Render::activeCamera->GetComponent<TransformComponent>();
			vec3 cameraPos = tc->GetPosition();
			model = translate(model, objectPos);

			mat4 rotationMatrix = lookAt(objectPos, cameraPos, vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = inverse(rotationMatrix);
			model = rotationMatrix;

			model = scale(model, obj->GetComponent<TransformComponent>()->GetScale());

			//bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *mat->GetTextureID(MaterialComponent::TextureType::diffuse));

			shader.SetMat4("model", model);
			auto mesh = obj->GetComponent<MeshComponent>();
			GLuint VAO = mesh->GetVAO();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	void Billboard::RenderBillboardUI(
		const shared_ptr<GameObject>& obj,
		const mat4& view,
		const mat4& projection)
	{
		if (obj == nullptr) Engine::CreateErrorPopup("Billboard gameobject is invalid.");

		if (obj->IsEnabled())
		{
			CorrectTransformValues(obj);
			string textureName = obj->GetComponent<MaterialComponent>()->GetTextureName(MaterialComponent::TextureType::diffuse);

			auto mat = obj->GetComponent<MaterialComponent>();

			Shader shader = mat->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			shader.SetFloat("transparency", 1.0f);
			shader.SetVec3("color", vec3(1));

			mat4 model = mat4(1.0f);

			vec3 pos = obj->GetComponent<TransformComponent>()->GetPosition();
			model = translate(model, pos);

			vec3 mscale = obj->GetComponent<TransformComponent>()->GetScale();
			model = scale(model, obj->GetComponent<TransformComponent>()->GetScale());

			//bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *mat->GetTextureID(MaterialComponent::TextureType::diffuse));

			shader.SetMat4("model", model);
			auto mesh = obj->GetComponent<MeshComponent>();
			GLuint VAO = mesh->GetVAO();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	void Billboard::CorrectTransformValues(const shared_ptr<GameObject>& obj)
	{
		vec3 pos = obj->GetComponent<TransformComponent>()->GetPosition();
		if (pos.z != 0.0f)
		{
			pos.z = 0.0f;

			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Warning: UI Billboard z position is not 0! Resetting back to 0.\n");

			obj->GetComponent<TransformComponent>()->SetPosition(pos);
		}

		vec3 scale = obj->GetComponent<TransformComponent>()->GetScale();
		if (scale.x > 10000.0f
			|| scale.y > 10000.0f)
		{
			scale.x = 200.0f;
			scale.y = 200.0f;

			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Warning: UI Billboard scale is too big! Resetting back to 200 for width and height.\n");

			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}
		if (scale.x <= 100.0f
			|| scale.y <= 100.0f)
		{
			scale.x = 200.0f;
			scale.y = 200.0f;

			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Warning: UI Billboard scale is 100 or below! Resetting back to 200 for width and height.\n");

			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}
		if (scale.z != 0.0f)
		{
			scale.z = 0.0f;

			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Warning: UI Billboard z scale is not 0! Resetting back to 0.\n");

			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}
	}
}