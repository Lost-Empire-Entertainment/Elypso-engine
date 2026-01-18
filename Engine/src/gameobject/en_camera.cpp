//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/en_core.hpp"

#include "gameobject/en_camera.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Core::EngineCore;

using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::quat;
using KalaHeaders::KalaMath::addpos;
using KalaHeaders::KalaMath::setpos;
using KalaHeaders::KalaMath::getpos;
using KalaHeaders::KalaMath::addrot;
using KalaHeaders::KalaMath::setrot;
using KalaHeaders::KalaMath::getroteuler;
using KalaHeaders::KalaMath::getdirfront;
using KalaHeaders::KalaMath::getdirright;
using KalaHeaders::KalaMath::getdirup;
using KalaHeaders::KalaMath::view;
using KalaHeaders::KalaMath::perspective;
using KalaHeaders::KalaMath::DIR_UP;

using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace ElypsoEngine::GameObject
{
	static EngineRegistry<Camera> registry{};

	EngineRegistry<Camera>& Camera::GetRegistry() { return registry; }

	Camera* Camera::Initialize(
		const string& cameraName,
		const vec3& pos,
		const vec3& rot,
		vec2 framebufferSize,
		f32 fov,
		f32 speed)
	{
		u32 newID = EngineCore::GetGlobalID() + 1;
		EngineCore::SetGlobalID(newID);

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

	bool Camera::IsInitialized() const { return isInitialized; }

	u32 Camera::GetID() const { return ID; }

	void Camera::SetName(const string& newName)
	{
		//skip if name is empty, same as existing or too long
		if (newName.empty()
			|| newName == name
			|| newName.length() > 50) return;

		name = newName;
	}
	const string& Camera::GetName() { return name; }

	void Camera::UpdateCameraRotation(vec2 delta)
	{
		vec3 inc{};

		//clamped pitch
		inc.x = clamp(-delta.y * sensitivity, -89.99f, 89.99f);
		//reglar yaw, already wrapped internally
		inc.y = -delta.x * sensitivity;
		//hard-locked roll
		inc.z = 0.0f;

		addrot(transform, {}, RotTarget::ROT_WORLD, inc);
	}

	void Camera::SetFOV(f32 newFOV)
	{
		fov = clamp(newFOV, 70.0f, 110.0f);
	}
	f32 Camera::GetFOV() const { return fov; }

	void Camera::SetNearClip(f32 newNearClip)
	{
		nearClip = clamp(newNearClip, 0.001f, farClip - 0.1f);
	}
	f32 Camera::GetNearClip() const { return nearClip; }

	void Camera::SetFarClip(f32 newFarClip)
	{
		farClip = clamp(newFarClip, nearClip + 0.1f, 1000.0f);
	}
	f32 Camera::GetFarClip() const { return farClip; }

	void Camera::SetSpeed(f32 newSpeed)
	{
		speed = clamp(newSpeed, 0.01f, 25.0f);
	}
	f32 Camera::GetSpeed() const { return speed; }

	void Camera::SetSensitivity(f32 newSens)
	{
		sensitivity = clamp(newSens, 0.001f, 10.0f);
	}
	f32 Camera::GetSensitivity() const { return sensitivity; }

	vec3 Camera::GetFront() { return getdirfront(transform); }
	vec3 Camera::GetRight() { return getdirright(transform); }
	vec3 Camera::GetUp() { return getdirup(transform); }

	void Camera::AddPos(const vec3& deltaPos)
	{
		addpos(
			transform,
			{},
			PosTarget::POS_WORLD,
			deltaPos);
	}
	void Camera::SetPos(const vec3& newPos)
	{
		setpos(
			transform,
			{},
			PosTarget::POS_WORLD,
			newPos);
	}
	vec3 Camera::GetPos()
	{
		return getpos(
			transform,
			PosTarget::POS_WORLD);
	}

	void Camera::AddRot(const vec3& deltaRot)
	{
		vec3 safeRot = vec3(
			clamp(deltaRot.x, -90.0f, 90.0f), //clamped pitch
			deltaRot.y,                       //regular yaw, already wrapped internally
			0.0f);                            //hard-locked roll

		addrot(
			transform,
			{},
			RotTarget::ROT_WORLD,
			safeRot);
	}
	void Camera::SetRot(const vec3& newRot)
	{
		vec3 safeRot = vec3(
			clamp(newRot.x, -90.0f, 90.0f), //clamped pitch
			newRot.y,                       //regular yaw, already wrapped internally
			0.0f);                          //hard-locked roll

		setrot(
			transform,
			{},
			RotTarget::ROT_WORLD,
			safeRot);
	}
	vec3 Camera::Camera::GetRot()
	{
		vec3 internalRot = getroteuler(
			transform,
			RotTarget::ROT_WORLD);

		return vec3(
			clamp(internalRot.x, -90.0f, 90.0f), //clamped pitch
			internalRot.y,                       //regular yaw, already wrapped internally
			0.0f);                               //hard-locked roll
	}

	mat4 Camera::GetViewMatrix()
	{
		vec3 pos = GetPos();
		vec3 front = GetFront();
		vec3 up = DIR_UP;

		return view(pos, pos + front, up);
	}
	mat4 Camera::GetOrthographicMatrix(vec2 viewportSize)
	{
		f32 nc = GetNearClip();
		f32 fc = GetFarClip();

		return ortho(viewportSize, nc, fc);
	}
	mat4 Camera::GetPerspectiveMatrix(vec2 viewportSize)
	{
		f32 fov = GetFOV();
		f32 nc = GetNearClip();
		f32 fc = GetFarClip();

		return perspective(viewportSize, fov, nc, fc);
	}

	Camera::~Camera()
	{
		Log::Print(
			"Destroying camera '" + name + "' with ID '" + to_string(ID) + "'.",
			"CAMERA",
			LogType::LOG_INFO);
	}
}