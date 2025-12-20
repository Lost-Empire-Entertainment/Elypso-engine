//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/kg_core.hpp"
#include "core/kg_camera.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace KalaGL::Core
{
	Camera* Camera::Initialize(
		const string& cameraName,
		const vec3& pos,
		const vec3& rot,
		vec2 framebufferSize,
		f32 fov,
		f32 speed)
	{
		u32 newID = ++KalaGLCore::globalID;
		unique_ptr<Camera> newCam = make_unique<Camera>();
		Camera* camPtr = newCam.get();

		Log::Print(
			"Creating camera '" + cameraName + "' with ID '" + to_string(newID) + "'.",
			"CAMERA",
			LogType::LOG_DEBUG);

		camPtr->SetFOV(fov);
		camPtr->SetSpeed(speed);

		camPtr->SetPos(pos);
		camPtr->SetRot(rot);
		camPtr->UpdateCameraRotation(vec2(0));

		registry.AddContent(newID, move(newCam));

		camPtr->name = cameraName;
		camPtr->ID = newID;

		camPtr->isInitialized = true;

		Log::Print(
			"Created camera '" + cameraName + "' with ID '" + to_string(newID) + "'!",
			"CAMERA",
			LogType::LOG_SUCCESS);

		return camPtr;
	}

	Camera::~Camera()
	{
		Log::Print(
			"Destroying camera '" + name + "' with ID '" + to_string(ID) + "'.",
			"CAMERA",
			LogType::LOG_INFO);
	}
}