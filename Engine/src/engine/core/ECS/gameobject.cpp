//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "glad.h"

//engine
#include "gameobject.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "render.hpp"

using std::make_shared;

using Core::Engine;
using Core::ECS::Create;
using ECSRender = Core::ECS::Render;
using Core::ECS::GameObject;
using Graphics::Shader;
using Graphics::Texture;
using GraphicsRender = Graphics::Render;

namespace Core::ECS
{
	Shader GameObjectShader;

	//
	// CREATE CUBE
	//

	GameObject Create::CreateCube(const vec3& position, const vec3& scale, const vec3& color, float shininess)
	{
		GameObject obj;
		obj.ID = obj.nextID++;

		auto transform = make_shared<Transform>();
		transform->position = position;
		transform->scale = scale;
		GameObject::AddComponent(transform);

		GameObjectShader = Shader(
			Engine::enginePath + "/shaders/GameObject.vert",
			Engine::enginePath + "/shaders/GameObject.frag");

		float vertices[] =
		{
			//positions          //normals           //texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		GLuint VAO;
		glGenVertexArrays(1, &VAO);
		GLuint VBO;
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		//position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//Texture tex(Engine::enginePath);
		//tex.LoadTexture("textures/crate_2.png", false, GL_RGBA);
		//tex.LoadTexture("textures/crate_2_specular.png", false, GL_RGBA);

		GameObjectShader.Use();
		GameObjectShader.SetInt("material.diffuse", 0);
		GameObjectShader.SetInt("material.specular", 1);

		GameObject::AddMeshComponent(vertices);

		auto material = make_shared<Material>(color, shininess, VAO, VBO);
		GameObject::AddMaterialComponent(
			material->GetColor(), 
			material->GetShininess(),
			material->GetVAO(),
			material->GetVBO());

		ECSRender::gameObjects.push_back(obj);

		return obj;
	}

	//
	// RENDER GAMEOBJECTS
	//

	void ECSRender::RenderCube(GameObject& obj, mat4& view, mat4& projection)
	{
		GameObjectShader.Use();
		GameObjectShader.SetVec3("viewPos", GraphicsRender::camera.GetCameraPosition());
		GameObjectShader.SetFloat("material.shininess", GraphicsRender::shininess);

		//directional light
		GameObjectShader.SetVec3("dirLight.direction", GraphicsRender::directionalDirection);
		GameObjectShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("dirLight.diffuse", GraphicsRender::directionalDiffuse);
		GameObjectShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		GameObjectShader.SetFloat("dirLight.intensity", GraphicsRender::directionalIntensity);
		//point light 1
		GameObjectShader.SetVec3("pointLights[0].position", GraphicsRender::pointLightPositions[0]);
		GameObjectShader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[0].diffuse", GraphicsRender::pointDiffuse);
		GameObjectShader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[0].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[0].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[0].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[0].intensity", GraphicsRender::pointIntensity);
		GameObjectShader.SetFloat("pointLights[0].distance", GraphicsRender::pointDistance);
		//point light 2
		GameObjectShader.SetVec3("pointLights[1].position", GraphicsRender::pointLightPositions[1]);
		GameObjectShader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[1].diffuse", GraphicsRender::pointDiffuse);
		GameObjectShader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[1].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[1].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[1].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[1].intensity", GraphicsRender::pointIntensity);
		GameObjectShader.SetFloat("pointLights[1].distance", GraphicsRender::pointDistance);
		//point light 3
		GameObjectShader.SetVec3("pointLights[2].position", GraphicsRender::pointLightPositions[2]);
		GameObjectShader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[2].diffuse", GraphicsRender::pointDiffuse);
		GameObjectShader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[2].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[2].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[2].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[2].intensity", GraphicsRender::pointIntensity);
		GameObjectShader.SetFloat("pointLights[2].distance", GraphicsRender::pointDistance);
		//point light 4
		GameObjectShader.SetVec3("pointLights[3].position", GraphicsRender::pointLightPositions[3]);
		GameObjectShader.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[3].diffuse", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[3].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[3].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[3].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[3].intensity", GraphicsRender::pointIntensity);
		GameObjectShader.SetFloat("pointLights[3].distance", GraphicsRender::pointDistance);
		//spotLight
		GameObjectShader.SetVec3("spotLight.position", GraphicsRender::camera.GetCameraPosition());
		GameObjectShader.SetVec3("spotLight.direction", GraphicsRender::camera.GetFront());
		GameObjectShader.SetFloat("spotLight.intensity", GraphicsRender::spotIntensity);
		GameObjectShader.SetFloat("spotLight.distance", GraphicsRender::spotDistance);
		GameObjectShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		GameObjectShader.SetVec3("spotLight.diffuse", GraphicsRender::spotDiffuse);
		GameObjectShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("spotLight.constant", 1.0f);
		GameObjectShader.SetFloat("spotLight.linear", 0.09f);
		GameObjectShader.SetFloat("spotLight.quadratic", 0.032f);
		GameObjectShader.SetFloat("spotLight.cutOff", cos(radians(GraphicsRender::spotInnerAngle)));
		GameObjectShader.SetFloat("spotLight.outerCutOff", cos(radians(GraphicsRender::spotOuterAngle)));

		GameObjectShader.SetMat4("projection", projection);
		GameObjectShader.SetMat4("view", view);

		mat4 model = mat4(1.0f);

		//bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[0]);
		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[1]);

		GameObjectShader.SetMat4("model", model);
		GLuint VAO = obj.GetComponent<Material>()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//
	// GAMEOBJECT
	//

	unsigned int GameObject::nextID = 1;
	unordered_map<type_index, shared_ptr<Component>> GameObject::components;

	GameObject::GameObject()
		: ID(nextID++)
	{}

	GameObject::GameObject(unsigned int GameObjectID) 
		: ID(GameObjectID)
	{}

	unsigned int GameObject::GetID() const
	{
		return ID;
	}

	void GameObject::AddMeshComponent(const float* vertices)
	{
		auto mesh = make_shared<Mesh>(vertices);
		AddComponent(mesh);
	}

	void GameObject::AddMaterialComponent(const vec3& color, float shininess, GLuint VAO, GLuint VBO)
	{
		auto material = make_shared<Material>(color, shininess, VAO, VBO);
		AddComponent(material);
	}

	//
	// MESH
	//

	template shared_ptr<Component> GameObject::GetComponent() const;

	Mesh::Mesh(const float* vertices)
		: vertices(vertices)
	{}

	const float* Mesh::GetVertices() const
	{
		return vertices;
	}

	//
	// MATERIAL
	//

	Material::Material(const vec3& color, float shininess, GLuint VAO, GLuint VBO)
		: color(color), shininess(shininess), VAO(VAO), VBO(VBO)
	{}

	const vec3& Material::GetColor() const
	{
		return color;
	}

	float Material::GetShininess() const
	{
		return shininess;
	}

	GLuint Material::GetVAO() const
	{
		return VAO;
	}

	GLuint Material::GetVBO() const
	{
		return VBO;
	}
}