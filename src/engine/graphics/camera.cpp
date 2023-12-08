//external
#include <glad.h>
#include <glm.hpp>
#include <matrix_transform.hpp>

//engine
#include "camera.h"

#include <vector>

using namespace glm;

namespace Graphics
{
    vec3 Camera::Position = glm::vec3(0.0f, 0.0f, 0.0f);
    vec3 Camera::Front = glm::vec3(0.0f, 0.0f, -1.0f);
    vec3 Camera::Up = glm::vec3(0.0f, 1.0f, 0.0f);
    vec3 Camera::Right = glm::vec3(0.0f);
    vec3 Camera::WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float Camera::Yaw = YAW;
    float Camera::Pitch = PITCH;
    float Camera::MovementSpeed = SPEED;
    float Camera::MouseSensitivity = SENSITIVITY;
    float Camera::Zoom = ZOOM;

    // Constructors
    Camera::Camera(
        glm::vec3 position,
        glm::vec3 up,
        float yaw,
        float pitch)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        UpdateCameraVectors();
    }

    Camera::Camera(
        float posX,
        float posY,
        float posZ,
        float upX,
        float upY,
        float upZ,
        float yaw,
        float pitch)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        UpdateCameraVectors();
    }

	mat4 Camera::GetViewMatrix() const
	{
		return lookAt(Position, Position + Front, Up);
	}

	void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= MouseSensitivity;
		yOffset *= MouseSensitivity;

		Yaw += xOffset;
		Pitch += yOffset;

		//prevent screen flipping when pitch is out of bounds
		if (constrainPitch)
		{
			if (Pitch > 89.0f) Pitch = 89.0f;
			if (Pitch < -89.0f) Pitch = -89.0f;
		}

		Camera::UpdateCameraVectors();
	}

	void Camera::ProcessMouseScroll(float yOffset)
	{
		Zoom -= (float)yOffset;
		if (Zoom < 1.0f) Zoom = 1.0f;
		if (Zoom > 90.0f) Zoom = 90.0f;
	}

	void Camera::UpdateCameraVectors()
	{
		//calculate the new front vector
		vec3 front{};
		front.x = cos(radians(Camera::Yaw)) * cos(radians(Camera::Pitch));
		front.y = sin(radians(Camera::Pitch));
		front.z = sin(radians(Camera::Yaw)) * cos(radians(Camera::Yaw));
		Front = normalize(front);
		//also re-calculate the right and up vector
		Camera::Right = normalize(cross(Camera::Front, Camera::WorldUp));
		Camera::Up = normalize(cross(Camera::Right, Camera::Front));
	}
}