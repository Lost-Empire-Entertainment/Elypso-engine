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
#include "render.hpp"
#include "cube.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::to_string;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using Graphics::Render;
using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using MeshType = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
using Core::Engine;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Cube::InitializeCube(const vec3& pos, const vec3& rot, const vec3& scale)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

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

		shared_ptr<Mesh> mesh = make_shared<Mesh>(MeshType::cube);

		Shader cubeShader = Shader(
			Engine::filesPath + "/shaders/GameObject.vert",
			Engine::filesPath + "/shaders/GameObject.frag");

		GLuint vao, vbo;

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

		shared_ptr<Material> mat = make_shared<Material>(cubeShader, vao, vbo);

		float shininess = 32;
		shared_ptr<BasicShape_Variables> basicShape = make_shared<BasicShape_Variables>(shininess);

		vector<unsigned int> textures;
		shared_ptr<GameObject> obj = make_shared<GameObject>(
			true, 
			"Cube", 
			GameObject::nextID++,
			transform, 
			mesh,
			mat,
			basicShape,
			textures);

		Texture tex(Engine::filesPath);
		tex.LoadTexture(obj, "textures/crate_2.png", false, GL_RGBA);
		tex.LoadTexture(obj, "textures/crate_2_specular.png", false, GL_RGBA);

		Shader assignedShader = obj->GetMaterial()->GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);
		assignedShader.SetInt("material.specular", 1);

		GameObjectManager::AddGameObject(obj);

		return obj;
	}

	void Cube::RenderCube(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
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

		//bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[0]);
		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[1]);

		shader.SetMat4("model", model);
		GLuint VAO = obj->GetMaterial()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}