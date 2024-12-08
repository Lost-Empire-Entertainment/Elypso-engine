//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <memory>
#include <iostream>

//external
#include "glm.hpp"
#include "magic_enum.hpp"

//engine
#include "lighttcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "core.hpp"
#include "billboard.hpp"
#include "selectobject.hpp"
#include "console.hpp"

using std::filesystem::path;
using glm::quat;
using glm::mat4;
using glm::vec3;
using std::make_shared;
using std::cout;

using Core::Engine;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::Billboard;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::Components
{
	void LightComponent::Initialize(
		const shared_ptr<GameObject>& parent, 
		const float* vertices,
		const string& meshType,
		const vec3& pos,
		const vec3& rot,
		const vec3& scale)
	{
		this->parent = parent;

		auto transform = make_shared<Transform>(pos, rot, scale);
		parent->SetTransform(transform);

		Mesh::MeshType type{};
		auto searchType = magic_enum::enum_cast<Mesh::MeshType>(meshType);
		if (searchType.has_value()) type = searchType.value();
		else
		{
			Engine::CreateErrorPopup(("Unknown mesh type '" + meshType + "' was provided for " + parent->GetName()).c_str());
		}

		if (type != Mesh::MeshType::point_light
			&& type != Mesh::MeshType::spot_light
			&& type != Mesh::MeshType::directional_light)
		{
			Engine::CreateErrorPopup(("Non-light mesh type '" + meshType + "' was provided for " + parent->GetName()).c_str());
		}
		else
		{
			switch (type)
			{
			case Mesh::MeshType::point_light:
			{
				auto mesh = parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::point_light);
				mesh->Initialize(Mesh::MeshType::point_light, vertices);
				break;
			}
			case Mesh::MeshType::spot_light:
			{
				auto mesh = parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::spot_light);
				mesh->Initialize(Mesh::MeshType::spot_light, vertices);
				break;
			}
			case Mesh::MeshType::directional_light:
			{
				auto mesh = parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::directional_light);
				mesh->Initialize(Mesh::MeshType::directional_light, vertices);
				break;
			}
			}

			auto material = parent->AddComponent<Material>();
			material->Initialize(
				(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
				(path(Engine::filesPath) / "shaders" / "Basic.frag").string());

			SetupBillboard(parent);
		}
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

		float transparency =
			(Select::selectedObj
				== parent.lock()
				&& Select::isObjectSelected)
			? 1.0f
			: 0.5f;
		shader.SetFloat("transparency", 1.0f);

		shader.SetVec3("color", lightColor);
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
			billboardDiffTexture,
			billboardShininess,
			billboardName,
			++GameObject::nextID,
			isBillboardEnabled);

		billboard->SetParent(parent);
		parent->AddChild(parent, billboard);
	}
}