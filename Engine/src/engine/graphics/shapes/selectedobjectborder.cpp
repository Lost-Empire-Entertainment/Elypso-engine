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
#include "selectedobjectborder.hpp"
#include "shader.hpp"
#include "core.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "console.hpp"

using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using std::filesystem::path;

using Graphics::Shader;
using Graphics::Components::Mesh;
using Type = Graphics::Components::Mesh::MeshType;
using Graphics::Components::Material;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Graphics::Render;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;

namespace Graphics::Shape
{
	shared_ptr<GameObject> Border::InitializeBorder(
		const vec3& pos, 
		const vec3& rot, 
		const vec3& scale)
	{
		auto obj = GameObject::Create(
			"Border",
			10000001,
			true);
		if (obj == nullptr) Engine::CreateErrorPopup("Failed to initialize border");

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

		auto transform = make_shared<Transform>(pos, rot, scale);
		obj->SetTransform(transform);

		auto mesh = obj->AddComponent<Mesh>(true, Mesh::MeshType::border);
		mesh->Initialize(Mesh::MeshType::border, vertices);

		auto material = obj->AddComponent<Material>();
		material->Initialize(
			(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
			(path(Engine::filesPath) / "shaders" / "Basic.frag").string());

		string objName = obj->GetName();
		if (obj->GetTransform() == nullptr) Engine::CreateErrorPopup(("Failed to assign transform component to " + objName).c_str());
		if (obj->GetComponent<Mesh>() == nullptr) Engine::CreateErrorPopup(("Failed to assign mesh component to " + objName).c_str());
		if (obj->GetComponent<Material>() == nullptr) Engine::CreateErrorPopup(("Failed to assign material component to '" + objName).c_str());

		GameObjectManager::SetBorder(obj);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			ConsoleType::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void Border::RenderBorder(
		const shared_ptr<GameObject>& obj, 
		const mat4& view, 
		const mat4& projection)
	{
		auto& transform = obj->GetTransform();
		auto material = obj->GetComponent<Material>();
		auto mesh = obj->GetComponent<Mesh>();
		if (!material
			|| !mesh)
		{
			return;
		}

		Shader shader = material->GetShader();

		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		shader.SetVec3("color", vec3(1.0));

		mat4 model = mat4(1.0f);

		if (Select::isObjectSelected)
		{
			auto& selectedObjTransform = Select::selectedObj->GetTransform();
			auto selectedObjMesh = Select::selectedObj->GetComponent<Mesh>();

			shader.SetFloat("transparency", 0.5f);

			if (selectedObjMesh->GetMeshType() == Mesh::MeshType::model)
			{
				//retrieve vertices and calculate bounding box
				const vector<AssimpVertex>& vertices = selectedObjMesh->GetVertices();
				vec3 minBound, maxBound;
				vec3 position = selectedObjTransform->GetPosition();
				vec3 initialScale = selectedObjTransform->GetScale();

				//calculate the bounding box based on vertices
				Select::CalculateInteractionBoxFromVertices(vertices, minBound, maxBound, position, initialScale);

				//compute the center and scale of the bounding box
				vec3 boxCenter = (minBound + maxBound) * 0.5f;
				vec3 boxScale = maxBound - minBound;

				//add a margin to the scale
				vec3 margin = vec3(0.1f);
				boxScale += margin;

				//translate to the center of the bounding box
				model = translate(model, boxCenter);

				//apply rotation
				quat newRot = quat(radians(selectedObjTransform->GetRotation()));
				model *= mat4_cast(newRot);

				//scale based on the bounding box size with the margin included
				model = scale(model, boxScale);
			}
			else
			{
				//simple position and margin values
				vec3 position = selectedObjTransform->GetPosition();

				//simple bounding box
				model = translate(model, position);

				//apply rotation
				quat newRot = quat(radians(selectedObjTransform->GetRotation()));
				model *= mat4_cast(newRot);

				//scale based on size, with a slight margin
				model = scale(model, vec3(1) + vec3(0.1f));
			}
		}
		else
		{
			shader.SetFloat("transparency", 0.0f);

			//move the border out of view when no object is selected
			model = translate(model, vec3(0.0f, -100.0f, 0.0f));
			model = scale(model, vec3(0.01f));
		}

		glLineWidth(5.0f);

		shader.SetMat4("model", model);

		GLuint VAO = mesh->GetVAO();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 24);

		glLineWidth(1.0f);
	}
}
#endif