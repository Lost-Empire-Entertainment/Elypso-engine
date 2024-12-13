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

using std::cout;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;
using std::filesystem::exists;

using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
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
		const string& diffTexture,
		unsigned int& id,
		const bool& isEnabled)
	{

		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

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

		shared_ptr<Mesh> mesh = make_shared<Mesh>(true, Type::billboard, vao, vbo, ebo);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_texture.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic_texture.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for selected object border is invalid!");
		}

		Shader billboardShader = Shader::LoadShader(vert, frag);

		shared_ptr<Material> mat = make_shared<Material>();
		mat->AddShader(vert, frag, billboardShader);

		float shininess = 32.0f;
		shared_ptr<BasicShape_Variables> basicShape = make_shared<BasicShape_Variables>(shininess);

		string billboardName = "Billboard";
		shared_ptr<GameObject> obj = make_shared<GameObject>(
			true,
			billboardName,
			id,
			isEnabled,
			transform,
			mesh,
			mat,
			basicShape);

		Texture::LoadTexture(obj, diffTexture, Material::TextureType::diffuse, true);

		Shader assignedShader = obj->GetMaterial()->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddTransparentObject(obj);
		GameObjectManager::AddBillboard(obj);

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

		if (GameObjectManager::renderBillboards
			&& obj->IsEnabled())
		{
			Shader shader = obj->GetMaterial()->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			shader.SetFloat("transparency", 1.0f);
			shader.SetVec3("color", vec3(1));

			mat4 model = mat4(1.0f);

			vec3 pos = parent->GetTransform()->GetPosition();
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
			glBindTexture(GL_TEXTURE_2D, obj->GetMaterial()->GetTextureID(Material::TextureType::diffuse));

			shader.SetMat4("model", model);
			GLuint VAO = obj->GetMesh()->GetVAO();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}