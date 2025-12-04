//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <sstream>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "core/kg_registry.hpp"

//TODO: figure out orthographic camera settings

namespace KalaGraphics::Core
{
	using std::string;
	using std::ostringstream;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::Transform3D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
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
	using KalaHeaders::KalaMath::DIR_UP;

	class LIB_API Camera
	{
	public:
		static inline KalaGraphicsRegistry<Camera> registry{};

		static Camera* Initialize(
			const string& cameraName,
			u32 windowID,
			const vec3& pos,
			const vec3& rot,
			vec2 framebufferSize,
			f32 fov,
			f32 speed);

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty, same as existing or too long
			if (newName.empty()
				|| newName == name
				|| newName.length() > 50) return;

			name = newName;
		}
		inline const string& GetName() { return name; }

		//Handle camera rotation based off of mouse movement
		void UpdateCameraRotation(vec2 delta)
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

		inline void SetFOV(f32 newFOV)
		{
			fov = clamp(newFOV, 70.0f, 110.0f);
		}
		inline f32 GetFOV() const { return fov; }

		inline void SetNearClip(f32 newNearClip)
		{
			nearClip = clamp(newNearClip, 0.001f, farClip - 0.1f);
		}
		inline f32 GetNearClip() const { return nearClip; }

		inline void SetFarClip(f32 newFarClip)
		{
			farClip = clamp(newFarClip, nearClip + 0.1f, 1000.0f);
		}
		inline f32 GetFarClip() const { return farClip; }

		inline void SetSpeed(f32 newSpeed)
		{
			speed = clamp(newSpeed, 0.01f, 25.0f);
		}
		inline f32 GetSpeed() const { return speed; }

		inline void SetSensitivity(f32 newSens)
		{
			sensitivity = clamp(newSens, 0.001f, 10.0f);
		}
		inline f32 GetSensitivity() const { return sensitivity; }
		
		inline vec3 GetFront() { return getdirfront(transform); }
		inline vec3 GetRight() { return getdirright(transform); }
		inline vec3 GetUp() { return getdirup(transform); }

		//Increments position over time
		inline void AddPos(const vec3& deltaPos)
		{
			addpos(
				transform,
				{},
				PosTarget::POS_WORLD,
				deltaPos);
		}
		//Snaps to given position
		inline void SetPos(const vec3& newPos)
		{
			setpos(
				transform,
				{},
				PosTarget::POS_WORLD,
				newPos);
		}
		inline vec3 GetPos() 
		{ 
			return getpos(
				transform,
				PosTarget::POS_WORLD); 
		}

		//Increments rotation over time
		inline void AddRot(const vec3& deltaRot)
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
		//Snaps to given rotation
		inline void SetRot(const vec3& newRot)
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
		inline vec3 GetRot() 
		{ 
			vec3 internalRot = getroteuler(
				transform,
				RotTarget::ROT_WORLD);
				
			return vec3(
				clamp(internalRot.x, -90.0f, 90.0f), //clamped pitch
				internalRot.y,                       //regular yaw, already wrapped internally
				0.0f);                               //hard-locked roll
		}
		
		mat4 GetViewMatrix()
		{
			vec3 pos = GetPos();
			vec3 front = GetFront();
			vec3 up = DIR_UP;
			
			ostringstream ss;

			ss  << "pos:   " << pos.x << ", " << pos.y << ", " << pos.z << "\n"
				<< "front: " << front.x << ", " << front.y << ", " << front.z << "\n"
				<< "up:    " << up.x << ", " << up.y << ", " << up.z << "\n";

			Log::Print(
				ss.str(),
				"VIEW_MATRIX_IN",
				LogType::LOG_DEBUG);
			
			return view(pos, pos + front, up);
		};
		//Returns aspect-ratio-correct ortographic matrix based off of inserted viewport
		mat4 GetOrthographicMatrix(vec2 viewportSize)
		{
			f32 nc = GetNearClip();
			f32 fc = GetFarClip();
			
			return ortho(viewportSize, nc, fc);
		}
		//Returns aspect-ratio-correct perspective matrix based off of inserted viewport
		mat4 GetPerspectiveMatrix(vec2 viewportSize)
		{
			f32 fov = GetFOV();
			f32 nc = GetNearClip();
			f32 fc = GetFarClip();
			
			ostringstream ss;

			ss  << "viewport:  " << viewportSize.x << ", " << viewportSize.y << "\n"
				<< "fov:       " << fov << "\n"
				<< "near clip: " << nc << "\n"
				<< "far clip:  " << fc << "\n";

			Log::Print(
				ss.str(),
				"PERSP_MATRIX_IN",
				LogType::LOG_DEBUG);
			
			return perspective(viewportSize, fov, nc, fc);
		}

		~Camera();
	private:
		bool isInitialized{};

		string name{};

		u32 ID{};
		u32 windowID{};

		f32 fov{};
		f32 speed{};

		f32 nearClip = 0.01f;
		f32 farClip = 512.0f;
		f32 sensitivity = 0.1f;

		Transform3D transform{};
	};
}