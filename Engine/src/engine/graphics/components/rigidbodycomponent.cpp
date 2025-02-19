//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <optional>

//external
#include "rigidbody.hpp"
#include "magic_enum.hpp"
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"

//engine
#include "rigidbodycomponent.hpp"
#include "console.hpp"
#include "physics.hpp"

using ElypsoPhysics::ColliderType;
using ElypsoPhysics::RigidBody;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

using Core::Physics;
using std::optional;
using glm::vec3;
using glm::quat;
using glm::radians;

namespace Graphics::Components
{
	RigidBodyComponent::RigidBodyComponent(
		shared_ptr<GameObject> owner,
		string colliderType,
		bool isDynamic,
		bool useGravity,
		float gravityFactor,
		float mass,
		float restitution,
		float staticFriction,
		float dynamicFriction) :
		isDynamic(isDynamic),
		useGravity(useGravity),
		gravityFactor(gravityFactor),
		mass(mass),
		restitution(restitution),
		staticFriction(staticFriction),
		dynamicFriction(dynamicFriction) 
	{
		ColliderType validType;
		auto type = magic_enum::enum_cast<ColliderType>(colliderType);
		if (!type.has_value())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Failed to initialize rigidbody because the colliderType '" + colliderType + "' is not a valid shape!\n");
		}
		else
		{
			SetOwner(owner);

			validType = type.value();
			
			auto transform = owner->GetComponent<TransformComponent>();
			vec3 pos = transform->GetPosition();

			vec3 eulerAngles = transform->GetRotation();
			eulerAngles = radians(eulerAngles);
			quat rot = quat(eulerAngles);

			vec3 scale = transform->GetScale();

			GameObjectHandle handle{};
			PhysicsWorld& physicsWorld = Physics::GetPhysicsWorld();
			
			handle = physicsWorld.CreateRigidBody(pos,
				rot,
				validType,
				scale,
				mass,
				restitution,
				staticFriction,
				dynamicFriction,
				gravityFactor,
				useGravity);

			if (handle.index == 0
				&& handle.generation == 0)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Failed to assign handle!\n");
			}
			else
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"Successfully assigned handle!\n");

				RigidBody* rb = physicsWorld.GetRigidBody(handle);

				if (rb == nullptr)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::INPUT,
						Type::EXCEPTION,
						"Error: Failed to initialize rigidbody!\n");
				}
				else
				{
					SetHandle(handle);

					ConsoleManager::WriteConsoleMessage(
						Caller::INPUT,
						Type::INFO,
						"Successfully initialized rigidbody!\n");
				}
			}
		}
	}

	void RigidBodyComponent::SetColliderType(const string& newColliderType)
	{
		optional<ColliderType> opt = magic_enum::enum_cast<ColliderType>(newColliderType);
		if (opt.has_value())
		{
			colliderType = string(magic_enum::enum_name(opt.value()));
		}
	}
}