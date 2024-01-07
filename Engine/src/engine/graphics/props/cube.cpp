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
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "cube.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "shader.hpp"
#include "render.hpp"
#include "texture.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using std::make_shared;
using std::to_string;

using Core::Engine;
using Core::ECS::GameObject;
using Core::ECS::Transform;
using Core::ECS::Material;
using Graphics::Shader;
using Graphics::Render;
using Graphics::Texture;

namespace Graphics::Props
{
	Shader GameObjectShader;

	GameObject Cube::CreateCube(const vec3& position, const vec3& scale, const vec3& color, float shininess)
	{
		GameObject obj;
		obj.SetName("Cube");
		obj.SetType(GameObject::Type::cube);

		GameObject::AddTransformComponent(position, vec3(0, 0, 0), scale);

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

		GameObject::AddMeshComponent(vertices);

		GLuint VAO{};
		glGenVertexArrays(1, &VAO);

		GLuint VBO{};
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		GameObject::AddMaterialComponent(color, shininess, VAO, VBO);

		//position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		Texture tex(Engine::enginePath);
		tex.LoadTexture("textures/crate_2.png", false, GL_RGBA);
		tex.LoadTexture("textures/crate_2_specular.png", false, GL_RGBA);

		GameObjectShader.Use();
		GameObjectShader.SetInt("material.diffuse", 0);
		GameObjectShader.SetInt("material.specular", 1);

		obj.Initialize();

		Render::gameObjects.push_back(obj);

		return obj;
	}

	void Cube::RenderCube(GameObject& obj, mat4& view, mat4& projection)
	{
		GameObjectShader.Use();
		GameObjectShader.SetVec3("viewPos", Render::camera.GetCameraPosition());
		GameObjectShader.SetFloat("material.shininess", Render::shininess);

		//directional light
		GameObjectShader.SetVec3("dirLight.direction", Render::directionalDirection);
		GameObjectShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("dirLight.diffuse", Render::directionalDiffuse);
		GameObjectShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		GameObjectShader.SetFloat("dirLight.intensity", Render::directionalIntensity);

		//point lights
		int pointLightCount = static_cast<int>(Render::pointLights.size());
		GameObjectShader.SetInt("numPointLights", pointLightCount);
		if (pointLightCount > 0)
		{
			for (int i = 0; i < pointLightCount; i++)
			{
				string lightPrefix = "pointLights[" + to_string(i) + "].";
				GameObjectShader.SetVec3(lightPrefix + "position", Render::pointLights[i].GetComponent<Transform>()->GetPosition());
				GameObjectShader.SetVec3(lightPrefix + "ambient", 0.05f, 0.05f, 0.05f);
				GameObjectShader.SetVec3(lightPrefix + "diffuse", Render::pointDiffuse);
				GameObjectShader.SetVec3(lightPrefix + "specular", 1.0f, 1.0f, 1.0f);
				GameObjectShader.SetFloat(lightPrefix + "constant", 1.0f);
				GameObjectShader.SetFloat(lightPrefix + "linear", 0.09f);
				GameObjectShader.SetFloat(lightPrefix + "quadratic", 0.032f);
				GameObjectShader.SetFloat(lightPrefix + "intensity", Render::pointIntensity);
				GameObjectShader.SetFloat(lightPrefix + "distance", Render::pointDistance);
			}
		}

		//spotLight
		GameObjectShader.SetVec3("spotLight.position", Render::camera.GetCameraPosition());
		GameObjectShader.SetVec3("spotLight.direction", Render::camera.GetFront());
		GameObjectShader.SetFloat("spotLight.intensity", Render::spotIntensity);
		GameObjectShader.SetFloat("spotLight.distance", Render::spotDistance);
		GameObjectShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		GameObjectShader.SetVec3("spotLight.diffuse", Render::spotDiffuse);
		GameObjectShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("spotLight.constant", 1.0f);
		GameObjectShader.SetFloat("spotLight.linear", 0.09f);
		GameObjectShader.SetFloat("spotLight.quadratic", 0.032f);
		GameObjectShader.SetFloat("spotLight.cutOff", cos(radians(Render::spotInnerAngle)));
		GameObjectShader.SetFloat("spotLight.outerCutOff", cos(radians(Render::spotOuterAngle)));

		GameObjectShader.SetMat4("projection", projection);
		GameObjectShader.SetMat4("view", view);

		mat4 model = mat4(1.0f);
		model = translate(model, obj.GetComponent<Transform>()->GetPosition());
		quat newRot = quat(radians(obj.GetComponent<Transform>()->GetRotation()));
		model *= mat4_cast(newRot);
		model = scale(model, obj.GetComponent<Transform>()->GetScale());

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
}