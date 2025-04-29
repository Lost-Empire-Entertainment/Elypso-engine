//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>
#include <string>
#include <iostream>

//external
#include "glm.hpp"

//engine
#include "game_core.hpp"
#include "game_asset.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "transformcomponent.hpp"
#include "rigidbodycomponent.hpp"
#include "cameracomponent.hpp"
#include "timeManager.hpp"

//game
#include "g_player.hpp"
#include "g_input.hpp"
#include "g_states.hpp"

using std::shared_ptr;
using std::string;
using glm::vec3;
using std::cout;
using std::min;
using glm::length;
using glm::normalize;
using glm::mix;

using Game::Game_Core;
using Game::Game_Asset;
using Graphics::Shape::GameObject;
using Core::Engine;
using Graphics::Components::TransformComponent;
using Graphics::Components::RigidBodyComponent;
using Graphics::Components::CameraComponent;
using Core::TimeManager;

namespace GameTemplate
{
	static vec3 lastRotation{};
	static bool startedHolding{};

	static shared_ptr<GameObject> model_player{};
	static shared_ptr<GameObject> camera{};

	static shared_ptr<TransformComponent> mtc{};
	static shared_ptr<TransformComponent> ctc{};
	static shared_ptr<RigidBodyComponent> mrc{};
	static shared_ptr<CameraComponent> cc{};

	void G_Player::Initialize()
	{
		//disable all movement but keep rotation
		Game_Core::SetRightClickState(G_States::rightClickState);
		Game_Core::SetYMovementState(G_States::yMovementState);
		Game_Core::SetRotationState(G_States::rotationState);
		Game_Core::SetMovementState(G_States::movementState);

		model_player = Game_Asset::GetGameObject("model_player");
		if (model_player == nullptr)
		{
			string errorMessage = "Failed to find model_player!";
			Engine::CreateErrorPopup(errorMessage.c_str());
			return;
		}

		camera = Game_Asset::GetCamera();
		if (camera == nullptr)
		{
			string errorMessage = "Failed to find camera!";
			Engine::CreateErrorPopup(errorMessage.c_str());
			return;
		}
	}

	void G_Player::UpdatePlayerPos()
	{
		if (mtc == nullptr)
		{
			mtc = model_player->GetComponent<TransformComponent>();
		}
		if (ctc == nullptr)
		{
			ctc = camera->GetComponent<TransformComponent>();
		}
		if (mrc == nullptr)
		{
			mrc = model_player->GetComponent<RigidBodyComponent>();
			if (mrc == nullptr)
			{
				string errorMessage = "Failed to find model_player rigidbody component!";
				Engine::CreateErrorPopup(errorMessage.c_str());
				return;
			}
		}

		//update camera pos to follow player model pos

		vec3 modelPos = mtc->GetPosition();
		ctc->SetPosition(modelPos);

		//lock player model angular velocity

		float avx = mrc->GetAngularVelocity().x;
		float avy = mrc->GetAngularVelocity().y;
		float avz = mrc->GetAngularVelocity().z;
		if (avx > 0.0f || avx < 0.0f) avx = 0.0f;
		if (avy > 0.0f || avy < 0.0f) avy = 0.0f;
		if (avz > 0.0f || avz < 0.0f) avz = 0.0f;

		MovePlayer();
	}

	void G_Player::MovePlayer()
	{
		if (G_States::canMove)
		{
			if (cc == nullptr)
			{
				cc = camera->GetComponent<CameraComponent>();
			}

			float currentSpeed = static_cast<float>(1.0f * TimeManager::deltaTime);

			vec3 front = cc->GetFront();
			vec3 right = cc->GetRight();
			vec3 impulse{};

			if (G_Input::IsHeld("Front")) impulse += front;
			if (G_Input::IsHeld("Back")) impulse -= front;
			if (G_Input::IsHeld("Right")) impulse += right;
			if (G_Input::IsHeld("Left")) impulse -= right;

			if (length(impulse) > 0.0f)
			{
				impulse = normalize(impulse);
				float desiredImpulse = currentSpeed * 1.0f;
				impulse *= desiredImpulse;

				vec3 currentVelocity = mrc->GetVelocity();
				vec3 horizontalVelocity = vec3(currentVelocity.x, 0.0f, currentVelocity.z);
				vec3 horizontalImpulse = vec3(impulse.x, 0.0f, impulse.z);

				vec3 newHorizontal = horizontalVelocity + horizontalImpulse;
				static float currentMaxVelocity = 2.0f;
				float targetMaxVelocity = G_Input::IsHeld("Sprint") ? 4.0f : 2.0f;
				currentMaxVelocity = mix(currentMaxVelocity, targetMaxVelocity, 0.1f);

				if (length(newHorizontal) > currentMaxVelocity)
				{
					newHorizontal = normalize(newHorizontal) * currentMaxVelocity;
					vec3 clampedImpulse = newHorizontal - horizontalVelocity;
					impulse.x = clampedImpulse.x;
					impulse.z = clampedImpulse.z;
				}
			}

			if (G_Input::IsPressed("Jump")
				&& abs(mrc->GetVelocity().y) < 0.1f)
			{
				impulse.y += 0.5f;
			}

			mrc->ApplyImpulse(impulse);

			//only reset velocity if not moving and not falling
			if (!G_Input::IsHeld("Front")
				&& !G_Input::IsHeld("Left")
				&& !G_Input::IsHeld("Back")
				&& !G_Input::IsHeld("Right")
				&& mrc->GetVelocity().x != 0.0f
				&& mrc->GetVelocity().y == 0.0f
				&& mrc->GetVelocity().z != 0.0f)
			{
				mrc->ResetVelocity();
			}
		}

		/*
		//fix player position if player falls off of map
		vec3 finalPos = mtc->GetPosition();
		if (finalPos.y < -10.0f
			|| finalPos.y > 100.0f
			|| finalPos.x < -1000.0f
			|| finalPos.x > 1000.0f
			|| finalPos.z < -1000.0f
			|| finalPos.z > 1000.0f)
		{
			mrc->SetDynamic(false);

			mtc->SetPosition(vec3(0.0f, 1.0f, 0.0f));
			ctc->SetPosition(vec3(0.0f, 1.0f, 0.0f));

			mrc->ResetVelocity();
			mrc->ResetAngularVelocity();

			mrc->SetDynamic(true);
		}
		*/
	}

	void G_Player::UpdateLastRotation()
	{
		/*
		if (cc == nullptr || ctc == nullptr) return;

		if (cc->IsEnabled()
			&& !startedHolding)
		{
			startedHolding = true;
		}
		if (!cc->IsEnabled()
			&& (startedHolding
			|| (!startedHolding
			&& cc->GetLastRotation() == vec3(0)
			&& ctc->GetRotation() != vec3(0))))
		{
			cc->SetLastRotation(ctc->GetRotation());
			startedHolding = false;
		}
		*/
	}
}