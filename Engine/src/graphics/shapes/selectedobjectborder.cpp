//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "selectedobjectborder.hpp"
#include "shader.hpp"
#include "core.hpp"
#include "render.hpp"
#include "selectobject.hpp"

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
using Core::Select;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Border::InitializeBorder(const vec3& pos, const vec3& rot, const vec3& scale)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

		float vertices[] =
		{
			//edges of the cube
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,

			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			//connecting edges
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
		};

		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		shared_ptr<Mesh> mesh = make_shared<Mesh>(Type::border, vao, vbo, ebo);

		Shader borderShader = Shader::LoadShader(
			Engine::filesPath + "\\shaders\\Basic_model.vert",
			Engine::filesPath + "\\shaders\\Basic.frag");

		shared_ptr<Material> mat = make_shared<Material>();
		mat->AddShader("shaders\\Basic_model.vert", "shaders\\Basic.frag", borderShader);

		float shininess = 32;
		shared_ptr<BasicShape_Variables> basicShape = make_shared<BasicShape_Variables>(shininess);

		shared_ptr<GameObject> obj = make_shared<GameObject>(
			false,
			"Border",
			10000000,
			true,
			transform,
			mesh,
			mat,
			basicShape);

		return obj;
	}

	void Border::RenderBorder(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		Shader shader = obj->GetMaterial()->GetShader();

		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		shader.SetVec3("color", vec3(1.0));

		mat4 model = mat4(1.0f);
		if (Select::isObjectSelected)
		{
			shader.SetFloat("transparency", 0.5f);

			model = translate(model, Select::selectedObj->GetTransform()->GetPosition());
			quat newRot = quat(radians(Select::selectedObj->GetTransform()->GetRotation()));
			model *= mat4_cast(newRot);
			model = scale(model, Select::selectedObj->GetTransform()->GetScale() + vec3(0.025f, 0.025f, 0.025f));
		}
		else
		{
			shader.SetFloat("transparency", 0.0f);

			model = translate(model, vec3(0.0f, -100.0f, 0.0f));
			quat newRot = quat(radians(vec3(0)));
			model *= mat4_cast(newRot);
			model = scale(model, vec3(0.01f));
		}

		shader.SetMat4("model", model);
		GLuint VAO = obj->GetMesh()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 24);
	}
}