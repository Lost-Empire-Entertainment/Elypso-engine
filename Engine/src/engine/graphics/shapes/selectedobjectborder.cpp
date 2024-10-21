//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
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

		shared_ptr<Mesh> mesh = make_shared<Mesh>(true, Type::border, vao, vbo, ebo);

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
			id,
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

			if (Select::selectedObj->GetMesh()->GetMeshType() == Mesh::MeshType::model)
			{
				//retrieve vertices and calculate bounding box
				const vector<AssimpVertex>& vertices = Select::selectedObj->GetMesh()->GetVertices();
				vec3 minBound, maxBound;
				vec3 position = Select::selectedObj->GetTransform()->GetPosition();
				vec3 initialScale = Select::selectedObj->GetTransform()->GetScale();
				float margin = 0.025f;

				//calculate the bounding box based on vertices
				Select::CalculateInteractionBoxFromVertices(vertices, minBound, maxBound, position, initialScale, margin);

				//compute the center and scale of the bounding box
				vec3 boxCenter = (minBound + maxBound) * 0.5f;
				vec3 boxScale = maxBound - minBound;

				model = translate(model, boxCenter); //translate to the center of the bounding box

				//apply rotation
				quat newRot = quat(radians(Select::selectedObj->GetTransform()->GetRotation()));
				model *= mat4_cast(newRot);

				//scale based on the bounding box size, with a slight margin
				model = scale(model, boxScale + vec3(margin));
			}
			else 
			{
				//simple position and margin values
				vec3 position = Select::selectedObj->GetTransform()->GetPosition();
				float margin = 0.025f;

				//simple bounding box
				model = translate(model, position);

				//apply rotation
				quat newRot = quat(radians(Select::selectedObj->GetTransform()->GetRotation()));
				model *= mat4_cast(newRot);

				//scale based on size, with a slight margin
				model = scale(model, vec3(1) + vec3(margin));
			}
		}
		else
		{
			shader.SetFloat("transparency", 0.0f);

			//move the border out of view when no object is selected
			model = translate(model, vec3(0.0f, -100.0f, 0.0f));
			model = scale(model, vec3(0.01f));
		}

		shader.SetMat4("model", model);

		GLuint VAO = obj->GetMesh()->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 24);
	}
}
#endif