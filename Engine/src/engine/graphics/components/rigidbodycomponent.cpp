//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <optional>

//external
#include "rigidbody.hpp"
#include "collider.hpp"
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
using ElypsoPhysics::Collider;
using ElypsoPhysics::BoxCollider;
using ElypsoPhysics::SphereCollider;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

using Core::Physics;
using std::optional;
using glm::vec3;
using glm::quat;
using glm::radians;
using std::to_string;

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
			PhysicsWorld* physicsWorld = Physics::physicsWorld;
			
			handle = physicsWorld->CreateRigidBody(
				pos,
				rot,
				validType,
				scale,
				mass,
				restitution,
				staticFriction,
				dynamicFriction,
				gravityFactor,
				useGravity);

			RigidBody* rb = physicsWorld->GetRigidBody(handle);

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

	void RigidBodyComponent::SetColliderType(const string& newColliderType)
	{
		optional<ColliderType> opt = magic_enum::enum_cast<ColliderType>(newColliderType);
		if (opt.has_value())
		{
			colliderType = string(magic_enum::enum_name(opt.value()));
		}
	}

	void RigidBodyComponent::SetPosition(const vec3& newPos)
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);

		rb->position = newPos;
	}
	void RigidBodyComponent::SetRotation(const vec3& newRot)
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);

		rb->rotation = quat(radians(newRot));
	}
	void RigidBodyComponent::SetScale(const vec3& newScale)
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		Collider* coll = rb->collider.get();

		if (coll->type == ColliderType::BOX)
		{
			BoxCollider* box = dynamic_cast<BoxCollider*>(coll);
			if (box)
			{
				box->halfExtents = newScale * 0.5f;
			}
		}
		else if (coll->type == ColliderType::SPHERE)
		{
			SphereCollider* sphere = dynamic_cast<SphereCollider*>(coll);
			if (sphere)
			{
				sphere->radius = newScale.x;
			}
		}
	}

	const vec3& RigidBodyComponent::GetPosition()
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);

		return rb->position;
	}
	const vec3& RigidBodyComponent::GetRotation()
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);

		vec3 newRot = degrees(eulerAngles(rb->rotation));
		return newRot;
	}
	const vec3& RigidBodyComponent::GetScale()
	{
		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		Collider* coll = rb->collider.get();

		if (coll->type == ColliderType::BOX)
		{
			BoxCollider* box = dynamic_cast<BoxCollider*>(coll);
			if (box)
			{
				return box->halfExtents * 2.0f;
			}
		}
		else if (coll->type == ColliderType::SPHERE)
		{
			SphereCollider* sphere = dynamic_cast<SphereCollider*>(coll);
			if (sphere)
			{
				return vec3(sphere->radius, 0.0f, 0.0f);
			}
		}
	}

	void RigidBodyComponent::SetDynamic(bool newIsDynamic)
	{
		isDynamic = newIsDynamic;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->isDynamic = isDynamic;
	}
	void RigidBodyComponent::EnableGravity(bool newUseGravity)
	{
		useGravity = newUseGravity;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->useGravity = useGravity;
	}
	void RigidBodyComponent::SetGravityFactor(float newGravityFactor)
	{
		gravityFactor = newGravityFactor;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->gravityFactor = gravityFactor;
	}
	void RigidBodyComponent::SetMass(float newMass)
	{
		mass = newMass;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->mass = mass;
	}
	void RigidBodyComponent::SetRestitution(float newRestitution)
	{
		restitution = newRestitution;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->restitution = restitution;
	}
	void RigidBodyComponent::SetStaticFriction(float newStaticFriction)
	{
		staticFriction = newStaticFriction;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->staticFriction = staticFriction;
	}
	void RigidBodyComponent::SetDynamicFriction(float newDynamicFriction)
	{
		dynamicFriction = newDynamicFriction;

		PhysicsWorld* physicsWorld = Physics::physicsWorld;
		RigidBody* rb = physicsWorld->GetRigidBody(handle);
		rb->dynamicFriction = dynamicFriction;
	}
}