//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <typeindex>

//external
#include "glm.hpp"
#include "glfw3.h"

//engine
#include "component.hpp"

namespace Graphics::Components
{
	using std::type_index;
	using glm::vec3;
	using glm::mat4;
	using glm::normalize;
	using glm::cross;
	using glm::radians;
	using std::shared_ptr;

	class CameraComponent : public Component
	{
	public:
		CameraComponent(
			bool isEnabled = false,
			float speed = 2.5f,
			float sensitivity = 0.05f,
			float yaw = -90.0f,
			float pitch = 0.0f,
			float lastX = 0.0f,
			float lastY = 0.0f,
			float aspectRatio = 0.0f,
			vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f),
			vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f),
			vec3 lastRotation = vec3(0));

		void RotateCamera(double xPos, double yPos);

		void SetEnableState(bool newEnableState) { isEnabled = newEnableState; }
		void SetSpeed(float newSpeed) { float speed = newSpeed; }
		void SetSensitivity(float newSensitivity) { sensitivity = newSensitivity; }
		void SetAspectRatio(float newAspectRatio) { aspectRatio = newAspectRatio; }
		void SetLastRotation(const vec3& newLastRotation) { lastRotation = newLastRotation; }

		void SetRotation(const vec3& newRotation)
		{
			//update internal state
			yaw = newRotation.x;
			pitch = newRotation.y;

			//recalculate cameraFront based on new yaw and pitch
			vec3 front{};
			front.x = cos(radians(yaw)) * cos(radians(pitch));
			front.y = sin(radians(pitch));
			front.z = sin(radians(yaw)) * cos(radians(pitch));
			cameraFront = normalize(front);
		}

		bool IsEnabled() const { return isEnabled; }
		float GetSpeed() const { return speed; }
		float GetSensitivity() const { return sensitivity; }
		float GetAspectRatio() const { return aspectRatio; }
		const vec3& GetLastRotation() const { return lastRotation; }

		vec3 GetFront() const { return cameraFront; }
		vec3 GetRight() const { return normalize(cross(cameraFront, cameraUp)); }
		vec3 GetUp() const { return cameraUp; }

		mat4 GetViewMatrix() const;

        type_index GetType() const override { return typeid(CameraComponent); }
	private:
		bool isEnabled;
		float speed;
		float sensitivity;
		float yaw;
		float pitch;
		float lastX;
		float lastY;
		float aspectRatio;
		vec3 cameraFront;
		vec3 cameraUp;
		vec3 lastRotation;
	};
}