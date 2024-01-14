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
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"

using std::to_string;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using Type = Graphics::Shape::Mesh::Type;
using Graphics::Shape::Material;
using Core::Engine;

namespace Graphics::Shape
{
	GameObject Cube::InitializeCube(const vec3& pos, const vec3& rot, const vec3& scale)
	{
		Transform transform(pos, rot, scale);

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

		Mesh mesh(Type::cube, vertices);

		Shader cubeShader = Shader(
			Engine::enginePath + "/shaders/GameObject.vert",
			Engine::enginePath + "/shaders/GameObject.frag");

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

		Material mat(cubeShader, vao, vbo);

		GameObject::nextID++;
		GameObject obj(false, GameObject::nextID, "Cube", transform, mesh, mat);

		Texture tex(Engine::enginePath);
		tex.LoadTexture("textures/crate_2.png", false, GL_RGBA);
		tex.LoadTexture("textures/crate_2_specular.png", false, GL_RGBA);

		Shader assignedShader = obj.GetMaterial().GetShader();
		assignedShader.Use();
		assignedShader.SetInt("material.diffuse", 0);
		assignedShader.SetInt("material.specular", 1);

		return obj;
	}

	void Cube::RenderCube(GameObject& obj, mat4& view, mat4& projection)
	{
		Shader shader = obj.GetMaterial().GetShader();

		shader.Use();
		shader.SetVec3("viewPos", Render::camera.GetCameraPosition());
		shader.SetFloat("material.shininess", Render::shininess);

		//directional light
		shader.SetVec3("dirLight.direction", Render::directionalDirection);
		shader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.SetVec3("dirLight.diffuse", Render::directionalDiffuse);
		shader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		shader.SetFloat("dirLight.intensity", Render::directionalIntensity);

		//point lights
		int pointLightCount = static_cast<int>(GameObject::pointLights.size());
		shader.SetInt("numPointLights", pointLightCount);
		if (pointLightCount > 0)
		{
			for (int i = 0; i < pointLightCount; i++)
			{
				string lightPrefix = "pointLights[" + to_string(i) + "].";
				shader.SetVec3(lightPrefix + "position", GameObject::pointLights[i].GetTransform().GetPosition());
				shader.SetVec3(lightPrefix + "ambient", 0.05f, 0.05f, 0.05f);
				shader.SetVec3(lightPrefix + "diffuse", Render::pointDiffuse);
				shader.SetVec3(lightPrefix + "specular", 1.0f, 1.0f, 1.0f);
				shader.SetFloat(lightPrefix + "constant", 1.0f);
				shader.SetFloat(lightPrefix + "linear", 0.09f);
				shader.SetFloat(lightPrefix + "quadratic", 0.032f);
				shader.SetFloat(lightPrefix + "intensity", Render::pointIntensity);
				shader.SetFloat(lightPrefix + "distance", Render::pointDistance);
			}
		}

		//spotLight
		shader.SetVec3("spotLight.position", Render::camera.GetCameraPosition());
		shader.SetVec3("spotLight.direction", Render::camera.GetFront());
		shader.SetFloat("spotLight.intensity", Render::spotIntensity);
		shader.SetFloat("spotLight.distance", Render::spotDistance);
		shader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shader.SetVec3("spotLight.diffuse", Render::spotDiffuse);
		shader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.SetFloat("spotLight.constant", 1.0f);
		shader.SetFloat("spotLight.linear", 0.09f);
		shader.SetFloat("spotLight.quadratic", 0.032f);
		shader.SetFloat("spotLight.cutOff", cos(radians(Render::spotInnerAngle)));
		shader.SetFloat("spotLight.outerCutOff", cos(radians(Render::spotOuterAngle)));

		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		mat4 model = mat4(1.0f);
		model = translate(model, obj.GetTransform().GetPosition());
		quat newRot = quat(radians(obj.GetTransform().GetRotation()));
		model *= mat4_cast(newRot);
		model = scale(model, obj.GetTransform().GetScale());

		//bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[0]);
		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[1]);

		shader.SetMat4("model", model);
		GLuint VAO = obj.GetMaterial().GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}