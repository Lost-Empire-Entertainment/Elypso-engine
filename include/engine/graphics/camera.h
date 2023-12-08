#pragma once

#ifndef GRAPHICS_CAMERA_H
#define GRAPHICS_CAMERA_H

//external
#include <glad.h>
#include <glm.hpp>
#include <matrix_transform.hpp>

#include <vector>

using namespace glm;

enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 90.0f;

namespace Graphics
{
	class Camera 
	{
	public:
		//camera attributes
		static vec3 Position;
		static vec3 Front;
		static vec3 Up;
		static vec3 Right;
		static vec3 WorldUp;
		//euler angles
		static float Yaw;
		static float Pitch;
		//camera options
		static float MovementSpeed;
		static float MouseSensitivity;
		static float Zoom;

		//constructor with vectors
		Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = YAW,
			float pitch = PITCH);
		//constructor with scalar values
		Camera(
			float posX,
			float posY,
			float posZ,
			float upX,
			float upY,
			float upZ,
			float yaw,
			float pitch);

		mat4 GetViewMatrix() const;

		static void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch);

		static void ProcessMouseScroll(float yOffset);

		static void UpdateCameraVectors();
	};
}

#endif