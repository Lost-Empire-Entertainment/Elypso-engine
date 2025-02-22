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
		ColliderType colliderType,
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
		SetOwner(owner);

		auto transform = owner->GetComponent<TransformComponent>();
		vec3 pos = transform->GetPosition();

		vec3 eulerAngles = transform->GetRotation();
		eulerAngles = radians(eulerAngles);
		quat rot = quat(eulerAngles);

		vec3 scale = transform->GetScale();

		GameObjectHandle handle{};
		PhysicsWorld* physicsWorld = Physics::physicsWorld;

		handle = physicsWorld->CreateRigidBody(
			vec3(0.0f),
			pos,
			vec3(0.0f),
			rot,
			colliderType,
			scale,
			mass,
			restitution,
			staticFriction,
			dynamicFriction,
			gravityFactor);

		if (handle.index == UINT32_MAX
			&& handle.generation == UINT32_MAX)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::EXCEPTION,
				"Error: Failed to initialize rigidbody! Did you forget to initialize Elypso Physics?\n");
		}
		else
		{
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
					Type::DEBUG,
					"Successfully initialized rigidbody!\n");
			}
		}
	}

	void RigidBodyComponent::ApplyForce(const vec3& force) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		rb->ApplyForce(force);
	}

	void RigidBodyComponent::ApplyImpulse(const vec3& impulse) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		rb->ApplyImpulse(impulse);
	}

	void RigidBodyComponent::ApplyTorque(const vec3& torque) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		rb->ApplyTorque(torque);
	}

	void RigidBodyComponent::SetColliderType(ColliderType colliderType) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		vec3 scale = GetOwner()->GetComponent<TransformComponent>()->GetScale();
		rb->SetCollider(vec3(0.0f), scale, colliderType);
	}

	void RigidBodyComponent::SetOffsetPosition(const vec3& newPos) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		if (rb->offsetPosition != newPos)
		{
			rb->offsetPosition = newPos;
		}
	}
	void RigidBodyComponent::SetCombinedPosition(const vec3& newPos) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		if (rb->combinedPosition != newPos)
		{
			rb->combinedPosition = newPos;
		}
	}

	void RigidBodyComponent::SetOffsetRotation(const vec3& newRot) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		quat newQuat = quat(radians(newRot));

		if (rb->offsetRotation != newQuat)
		{
			rb->offsetRotation = quat(radians(newRot));
		}
	}
	void RigidBodyComponent::SetCombinedRotation(const vec3& newRot) const
	{
		RigidBody* rb =  Physics::physicsWorld->GetRigidBody(handle);
		quat newQuat = quat(radians(newRot));

		if (rb->combinedRotation != newQuat)
		{
			rb->combinedRotation = quat(radians(newRot));
		}
	}

	void RigidBodyComponent::SetOffsetScale(const vec3& newScale) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		Collider* collider = rb->collider;

		if (collider->offsetScale != newScale)
		{
			collider->offsetScale = newScale;
		}
	}
	void RigidBodyComponent::SetCombinedScale(const vec3& newScale) const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		Collider* collider = rb->collider;

		if (collider->combinedScale != newScale)
		{
			collider->combinedScale = newScale;
			collider->UpdateScale(newScale);
		}
	}

	void RigidBodyComponent::ResetVelocity() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		rb->velocity = vec3(0);
	}
	void RigidBodyComponent::ResetAngularVelocity() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		rb->angularVelocity = vec3(0);
	}

	vec3 RigidBodyComponent::GetOffsetPosition() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		return rb->offsetPosition;
	}
	vec3 RigidBodyComponent::GetCombinedPosition() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		return rb->combinedPosition;
	}

	vec3 RigidBodyComponent::GetOffsetRotation() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		vec3 newRot = degrees(eulerAngles(rb->offsetRotation));
		return newRot;
	}
	vec3 RigidBodyComponent::GetCombinedRotation() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);

		vec3 newRot = degrees(eulerAngles(rb->combinedRotation));
		return newRot;
	}

	vec3 RigidBodyComponent::GetOffsetScale() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		Collider* collider = rb->collider;
		return collider->offsetScale;
	}
	vec3 RigidBodyComponent::GetCombinedScale() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		Collider* collider = rb->collider;
		return collider->combinedScale;
	}

	vec3 RigidBodyComponent::GetVelocity() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) return rb->velocity;
		else return vec3(0);
	}
	vec3 RigidBodyComponent::GetAngularVelocity() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) return rb->angularVelocity;
		else return vec3(0);
	}

	ColliderType RigidBodyComponent::GetColliderType() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr)
		{
			return rb->collider->type;
		}
		else return ColliderType::BOX;
	}

	Collider* RigidBodyComponent::GetCollider() const
	{
		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		return rb->collider;
	}

	void RigidBodyComponent::SetDynamic(bool newIsDynamic)
	{
		isDynamic = newIsDynamic;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->isDynamic = isDynamic;
	}
	void RigidBodyComponent::EnableGravity(bool newUseGravity)
	{
		useGravity = newUseGravity;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->useGravity = useGravity;
	}
	void RigidBodyComponent::SetGravityFactor(float newGravityFactor)
	{
		gravityFactor = newGravityFactor;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->gravityFactor = gravityFactor;
	}
	void RigidBodyComponent::SetMass(float newMass)
	{
		mass = newMass;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->mass = mass;
	}
	void RigidBodyComponent::SetRestitution(float newRestitution)
	{
		restitution = newRestitution;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->restitution = restitution;
	}
	void RigidBodyComponent::SetStaticFriction(float newStaticFriction)
	{
		staticFriction = newStaticFriction;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->staticFriction = staticFriction;
	}
	void RigidBodyComponent::SetDynamicFriction(float newDynamicFriction)
	{
		dynamicFriction = newDynamicFriction;

		RigidBody* rb = Physics::physicsWorld->GetRigidBody(handle);
		if (rb != nullptr) rb->dynamicFriction = dynamicFriction;
	}
}