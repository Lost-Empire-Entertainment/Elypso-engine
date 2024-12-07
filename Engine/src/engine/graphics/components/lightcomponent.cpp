//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <memory>
#include <iostream>

//external
#include "glm.hpp"

//engine
#include "lighttcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "core.hpp"
#include "billboard.hpp"

using std::filesystem::path;
using glm::quat;
using glm::mat4;
using glm::vec3;
using std::make_shared;
using std::cout;

using Core::Engine;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::Billboard;

namespace Graphics::Components
{
	void LightComponent::Initialize(
		const shared_ptr<GameObject>& parent, 
		const float* vertices,
		const vec3& pos,
		const vec3& rot,
		const vec3& scale)
	{
		this->parent = parent;

		auto transform = make_shared<Transform>(pos, rot, scale);
		parent->SetTransform(transform);

		auto mesh = parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::point_light);
		mesh->Initialize(Mesh::MeshType::point_light, vertices, sizeof(vertices));

		auto material = parent->AddComponent<Material>();
		material->Initialize(
			(path(Engine::filesPath) / "shaders" / vertShader).string(),
			(path(Engine::filesPath) / "shaders" / fragShader).string());

		SetupBillboard(parent);
	}

	void LightComponent::Render(
		const mat4& view, 
		const mat4& projection)
	{
		auto parentPtr = parent.lock();
		if (!parentPtr
			|| !parentPtr->IsEnabled())
		{
			return;
		}

		auto material = parentPtr->GetComponent<Material>();
		auto mesh = parentPtr->GetComponent<Mesh>();
		auto& transform = parentPtr->GetTransform();

		if (material
			&& mesh
			&& transform)
		{
			Shader shader = material->GetShader();

			CastLight(
				shader,
				view,
				projection,
				GetDiffuse());

			if (GameObjectManager::renderLightBorders
				&& mesh->IsEnabled())
			{
				RenderBorder(
					transform,
					shader,
					mesh->GetVAO(),
					GetVertexCount());
			}
		}
	}

	void LightComponent::CastLight(
		Shader& shader, 
		const mat4& view, 
		const mat4& projection, 
		const vec3& lightColor) const
	{
		shader.Use();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);
		shader.SetVec3("color", lightColor);

		//shared_ptr<GameObject> par = parent.lock();
		//cout << "rendering " << par->GetName() << "\n";

		/*
		float transparency =
			(Select::selectedObj
				== parent.lock()
				&& Select::isObjectSelected)
			? 1.0f
			: 0.5f;
		*/
		shader.SetFloat("transparency", 1.0f);
	}

	void LightComponent::RenderBorder(
		const shared_ptr<Transform>& transform, 
		Shader& shader, 
		GLuint vao, 
		int vertexCount)
	{
		mat4 model = mat4(1.0f);
		model = translate(model, transform->GetPosition());
		quat newRot = quat(radians(transform->GetRotation()));
		model *= mat4_cast(newRot);
		model = scale(model, transform->GetScale());

		shader.SetMat4("model", model);

		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void LightComponent::SetupBillboard(const shared_ptr<GameObject>& parent) const
	{
		string billboardName = parent->GetName() + " billboard";
		auto billboard = Billboard::InitializeBillboard(
			parent->GetTransform()->GetPosition(),
			parent->GetTransform()->GetRotation(),
			parent->GetTransform()->GetScale(),
			billboardVertShader,
			billboardFragShader,
			billboardDiffTexture,
			billboardShininess,
			billboardName,
			++GameObject::nextID,
			isBillboardEnabled);

		billboard->SetParent(parent);
		parent->AddChild(parent, billboard);
	}
}