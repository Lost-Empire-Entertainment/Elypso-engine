//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "transformcomponent.hpp"
#include "gameobject.hpp"
#include "cameracomponent.hpp"
#include "meshcomponent.hpp"

using Graphics::Shape::GameObject;

namespace Graphics::Components
{
	void TransformComponent::SetRotation(const vec3& newRotation)
	{
		rotation = newRotation;

		auto owner = GetOwner();
		auto mc = owner->GetComponent<MeshComponent>();
		if (mc == nullptr) return;

		auto type = mc->GetMeshType();
		if (type == MeshComponent::MeshType::camera)
		{
			auto cc = owner->GetComponent<CameraComponent>();
			if (cc->GetRotation() != newRotation)
			{
				cc->SetRotation(newRotation);
			}
		}
	}
}