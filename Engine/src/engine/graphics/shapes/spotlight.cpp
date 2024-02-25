//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
#include "spotlight.hpp"
#include "shader.hpp"
#include "core.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "billboard.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;

using Graphics::Shader;
using Graphics::Shape::Mesh;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Graphics::Render;
using Physics::Select;

namespace Graphics::Shape
{
	shared_ptr<GameObject> SpotLight::InitializeSpotLight(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& vertShader,
		const string& fragShader,
		const vec3& diffuse,
		const float& intensity,
		const float& distance,
		const float& innerAngle,
		const float& outerAngle,
		string& name,
		unsigned int& id,
		
		const string& billboardVertShader,
		const string& billboardFragShader,
		const string& billboardDiffTexture,
		const float& billboardShininess,
		string& billboardName,
		unsigned int& billboardID)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

		float vertices[] =
		{
			//four corner edges
			0.0f,  0.5f,  0.0f, 
		   -0.5f, -0.5f, -0.5f,

			0.0f,  0.5f,  0.0f,
		    0.5f, -0.5f, -0.5f,

			0.0f,  0.5f,  0.0f,
		   -0.5f, -0.5f,  0.5f,

			0.0f,  0.5f,  0.0f,
		    0.5f, -0.5f,  0.5f,

			//four bottom edges
			0.5f, -0.5f,  0.5f,
		   -0.5f, -0.5f,  0.5f,

		    0.5f, -0.5f, -0.5f,
		   -0.5f, -0.5f, -0.5f,

		   -0.5f, -0.5f, -0.5f,
		   -0.5f, -0.5f,  0.5f,

		    0.5f, -0.5f, -0.5f,
		    0.5f, -0.5f,  0.5f
		};

		shared_ptr<Mesh> mesh = make_shared<Mesh>(Type::spot_light);

		Shader spotlightShader = Shader(
			Engine::enginePath + "/" + vertShader,
			Engine::enginePath + "/" + fragShader);

		GLuint vao, vbo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		shared_ptr<Material> mat = make_shared<Material>(vao, vbo);
		mat->AddShader(vertShader, fragShader, spotlightShader);

		shared_ptr<SpotLight_Variables> spotLight = 
			make_shared<SpotLight_Variables>(
				diffuse, 
				intensity, 
				distance,
				innerAngle,
				outerAngle);

		shared_ptr<GameObject> billboard = Billboard::InitializeBillboard(
			pos, 
			rot, 
			scale, 
			billboardVertShader,
			billboardFragShader,
			billboardDiffTexture,
			billboardShininess,
			billboardName,
			billboardID);

		if (name == tempName) name = "Spotlight";
		if (id == tempID) id = GameObject::nextID++;
		shared_ptr<GameObject> obj = make_shared<GameObject>(
			true,
			name,
			id,
			transform,
			mesh,
			mat,
			spotLight);

		billboard->SetParentBillboardHolder(obj);
		obj->SetChildBillboard(billboard);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);
		GameObjectManager::AddSpotLight(obj);

		return obj;
	}

	void SpotLight::RenderSpotLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		Shader shader = obj->GetMaterial()->GetShader();

		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		float transparency = Select::selectedObj == 
			obj 
			&& Select::isObjectSelected ? 1.0f : 0.5f;
		shader.SetFloat("transparency", transparency);
		shader.SetVec3("color", obj->GetSpotLight()->GetDiffuse());

		mat4 model = mat4(1.0f);
		model = translate(model, obj->GetTransform()->GetPosition());
		quat newRot = quat(radians(obj->GetTransform()->GetRotation()));
		model *= mat4_cast(newRot);
		model = scale(model, obj->GetTransform()->GetScale());

		shader.SetMat4("model", model);
		GLuint VAO = obj->GetMaterial()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 32);
	}
}