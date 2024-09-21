//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Graphics::Render;
using Core::Select;
using Core::Input;

namespace Graphics::Shape
{
	shared_ptr<GameObject> ActionTex::InitializeActionTex(const vec3& pos, const vec3& rot, const vec3& scale)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

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

		shared_ptr<Mesh> mesh = make_shared<Mesh>(Type::actionTex, vao, vbo, ebo);

		Shader borderShader = Shader::LoadShader(
			Engine::filesPath + "\\shaders\\Basic_texture.vert",
			Engine::filesPath + "\\shaders\\Basic_texture.frag");

		shared_ptr<Material> mat = make_shared<Material>();
		mat->AddShader("shaders\\Basic_texture.vert", "shaders\\Basic_texture.frag", borderShader);

		float shininess = 32;
		shared_ptr<BasicShape_Variables> basicShape = make_shared<BasicShape_Variables>(shininess);

		shared_ptr<GameObject> obj = make_shared<GameObject>(
			false,
			"ActionTex",
			id,
			true,
			transform,
			mesh,
			mat,
			basicShape);

		Texture::LoadTexture(obj, Engine::filesPath + "\\icons\\blank.png", Material::TextureType::misc_icon_blank, true);
		Texture::LoadTexture(obj, Engine::filesPath + "\\icons\\move.png", Material::TextureType::misc_icon_move, true);
		Texture::LoadTexture(obj, Engine::filesPath + "\\icons\\rotate.png", Material::TextureType::misc_icon_rotate, true);
		Texture::LoadTexture(obj, Engine::filesPath + "\\icons\\scale.png", Material::TextureType::misc_icon_scale, true);

		Shader assignedShader = obj->GetMaterial()->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);

		return obj;
	}

	void ActionTex::RenderActionTex(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		Shader shader = obj->GetMaterial()->GetShader();

		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		shader.SetVec3("color", vec3(1.0));

		mat4 model = mat4(1.0f);
		if (Select::isObjectSelected)
		{
			shader.SetFloat("transparency", 1.0f);

			if (Input::axis == "X")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(0.0f, 1.5f, 0.0f));
					quat newRot = quat(radians(vec3(90.0f, 0.0f, 0.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(-1.5f, 0.0f, 0.0f));
					quat newRot = quat(radians(vec3(90.0f, 0.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
			}
			else if (Input::axis == "Y")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(0.0f, 0.0f, -1.5f));
					quat newRot = quat(radians(vec3(0.0f, 0.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(0.0f, -1.5f, 0.0f));
					quat newRot = quat(radians(vec3(0.0f, 90.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
			}
			else if (Input::axis == "Z")
			{
				if (Input::objectAction == Input::ObjectAction::move
					|| Input::objectAction == Input::ObjectAction::scale)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(0.0f, -1.5f, 0.0f));
					quat newRot = quat(radians(vec3(0.0f, 90.0f, 90.0f)));
					model *= mat4_cast(newRot);
				}
				else if (Input::objectAction == Input::ObjectAction::rotate)
				{
					model = translate(model, Select::selectedObj->GetTransform()->GetPosition() - vec3(0.0f, 0.0f, -1.5f));
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
			glBindTexture(GL_TEXTURE_2D, obj->GetMaterial()->GetTextureID(Material::TextureType::misc_icon_blank));
		}
		else if (Input::objectAction == Input::ObjectAction::move)
		{
			glBindTexture(GL_TEXTURE_2D, obj->GetMaterial()->GetTextureID(Material::TextureType::misc_icon_move));
		}
		else if (Input::objectAction == Input::ObjectAction::rotate)
		{
			glBindTexture(GL_TEXTURE_2D, obj->GetMaterial()->GetTextureID(Material::TextureType::misc_icon_rotate));
		}
		else if (Input::objectAction == Input::ObjectAction::scale)
		{
			glBindTexture(GL_TEXTURE_2D, obj->GetMaterial()->GetTextureID(Material::TextureType::misc_icon_scale));
		}

		shader.SetMat4("model", model);
		GLuint VAO = obj->GetMesh()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}