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
#include "console.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;
using std::filesystem::exists;

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
		auto obj = make_shared<GameObject>("SelectedObjectAction", 10000002, "");
		auto& transform = obj->GetTransform();
		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scale);
		obj->SetEnableState(false);

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
			true, 
			MeshType::actionTex, 
			vao, 
			vbo, 
			ebo);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for selected object action is invalid!");
		}

		Shader borderShader = Shader::LoadShader(vert, frag);

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->AddShader(vert, frag ,borderShader);

		string blankTex = (path(Engine::filesPath) / "icons" / "blank.png").string();
		string moveTex = (path(Engine::filesPath) / "icons" / "move.png").string();
		string rotateTex = (path(Engine::filesPath) / "icons" / "rotate.png").string();
		string scaleTex = (path(Engine::filesPath) / "icons" / "scale.png").string();

		Texture::LoadTexture(obj, blankTex, MaterialComponent::TextureType::misc_icon_blank, true);
		Texture::LoadTexture(obj, moveTex, MaterialComponent::TextureType::misc_icon_move, true);
		Texture::LoadTexture(obj, rotateTex, MaterialComponent::TextureType::misc_icon_rotate, true);
		Texture::LoadTexture(obj, scaleTex, MaterialComponent::TextureType::misc_icon_scale, true);

		Shader assignedShader = mat->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);

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
		if (obj == nullptr) Engine::CreateErrorPopup("Action texture gameobject is invalid.");

		auto mat = obj->GetComponent<MaterialComponent>();

		Shader shader = mat->GetShader();

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
			glBindTexture(GL_TEXTURE_2D, mat->GetTextureID(MaterialComponent::TextureType::misc_icon_blank));
		}
		else if (Input::objectAction == Input::ObjectAction::move)
		{
			glBindTexture(GL_TEXTURE_2D, mat->GetTextureID(MaterialComponent::TextureType::misc_icon_move));
		}
		else if (Input::objectAction == Input::ObjectAction::rotate)
		{
			glBindTexture(GL_TEXTURE_2D, mat->GetTextureID(MaterialComponent::TextureType::misc_icon_rotate));
		}
		else if (Input::objectAction == Input::ObjectAction::scale)
		{
			glBindTexture(GL_TEXTURE_2D, mat->GetTextureID(MaterialComponent::TextureType::misc_icon_scale));
		}

		shader.SetMat4("model", model);
		auto mesh = obj->GetComponent<MeshComponent>();
		GLuint VAO = mesh->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
#endif