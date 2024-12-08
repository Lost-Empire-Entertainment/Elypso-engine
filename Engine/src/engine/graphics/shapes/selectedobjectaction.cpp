//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE

#include <filesystem>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "selectedobjectaction.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "input.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "console.hpp"

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
using Core::Input;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;

namespace Graphics::Shape
{
	shared_ptr<GameObject> ActionTex::InitializeActionTex(
		const vec3& pos, 
		const vec3& rot, 
		const vec3& scale)
	{
		auto obj = GameObject::Create(
			"ActionTex", 
			10000002, 
			true);

		float vertices[] =
		{
			// positions             // normals            // texture coords
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
			 0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // Bottom-right
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // Top-right
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // Top-right
			-0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 1.0f, // Top-left
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
		};

		auto transform = make_shared<Transform>(pos, rot, scale);
		obj->SetTransform(transform);

		auto mesh = obj->AddComponent<Mesh>(true, Mesh::MeshType::actionTex);
		mesh->Initialize(Mesh::MeshType::actionTex, vertices);

		auto material = obj->AddComponent<Material>();
		material->Initialize(
			(path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string(),
			(path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string());

		string objName = obj->GetName();
		if (obj->GetTransform() == nullptr) Engine::CreateErrorPopup(("Failed to assign transform component to " + objName).c_str());
		if (obj->GetComponent<Mesh>() == nullptr) Engine::CreateErrorPopup(("Failed to assign mesh component to " + objName).c_str());
		if (obj->GetComponent<Material>() == nullptr) Engine::CreateErrorPopup(("Failed to assign material component to '" + objName).c_str());

		//load selected object action textures
		Texture::LoadTexture(obj, (path(Engine::filesPath) / "icons" / "blank.png").string(), Material::TextureType::misc_icon_blank, true);
		Texture::LoadTexture(obj, (path(Engine::filesPath) / "icons" / "move.png").string(), Material::TextureType::misc_icon_move, true);
		Texture::LoadTexture(obj, (path(Engine::filesPath) / "icons" / "rotate.png").string(), Material::TextureType::misc_icon_rotate, true);
		Texture::LoadTexture(obj, (path(Engine::filesPath) / "icons" / "scale.png").string(), Material::TextureType::misc_icon_scale, true);

		GameObjectManager::SetActionTex(obj);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			ConsoleType::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void ActionTex::RenderActionTex(
		const shared_ptr<GameObject>& obj, 
		const mat4& view, 
		const mat4& projection)
	{
		auto& transform = obj->GetTransform();
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

		shader.SetVec3("color", vec3(1.0));

		mat4 model = mat4(1.0f);
		if (Select::isObjectSelected)
		{
			auto& selectedObjTransform = Select::selectedObj->GetTransform();

			shader.SetFloat("transparency", 1.0f);

			if (Input::axis == "X")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(0.0f, 1.5f, 0.0f));
					quat newRot = quat(radians(vec3(90.0f, 0.0f, 0.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(-1.5f, 0.0f, 0.0f));
					quat newRot = quat(radians(vec3(90.0f, 0.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
			}
			else if (Input::axis == "Y")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(0.0f, 0.0f, -1.5f));
					quat newRot = quat(radians(vec3(0.0f, 0.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(0.0f, -1.5f, 0.0f));
					quat newRot = quat(radians(vec3(0.0f, 90.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
			}
			else if (Input::axis == "Z")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(0.0f, -1.5f, 0.0f));
					quat newRot = quat(radians(vec3(0.0f, 90.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, selectedObjTransform->GetPosition() - vec3(0.0f, 0.0f, -1.5f));
					quat newRot = quat(radians(vec3(0.0f, 0.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
			}

			model = scale(model, vec3(3));
		}
		else
		{
			shader.SetFloat("transparency", 0.0f);

			model = translate(model, vec3(0.0f, -100.0f, 0.0f));
			quat newRot = quat(radians(vec3(0)));
			model *= mat4_cast(newRot);
			model = scale(model, vec3(0.01f));
		}

		//bind texture
		glActiveTexture(GL_TEXTURE0);
		if (Input::objectAction == Input::ObjectAction::none)
		{
			glBindTexture(GL_TEXTURE_2D, material->GetTextureID(Material::TextureType::misc_icon_blank));
		}
		else if (Input::objectAction == Input::ObjectAction::move)
		{
			glBindTexture(GL_TEXTURE_2D, material->GetTextureID(Material::TextureType::misc_icon_move));
		}
		else if (Input::objectAction == Input::ObjectAction::rotate)
		{
			glBindTexture(GL_TEXTURE_2D, material->GetTextureID(Material::TextureType::misc_icon_rotate));
		}
		else if (Input::objectAction == Input::ObjectAction::scale)
		{
			glBindTexture(GL_TEXTURE_2D, material->GetTextureID(Material::TextureType::misc_icon_scale));
		}

		shader.SetMat4("model", model);
		GLuint VAO = mesh->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
#endif