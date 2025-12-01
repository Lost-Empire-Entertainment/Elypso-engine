//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "utils/kg_registry.hpp"

namespace KalaGraphics::Graphics
{
	using std::string;

	using KalaHeaders::vec2;
	using KalaHeaders::vec3;
	using KalaHeaders::mat4;
	using KalaHeaders::quat;
	using KalaHeaders::Transform3D;
	using KalaHeaders::PosTarget;
	using KalaHeaders::RotTarget;
	using KalaHeaders::addpos;
	using KalaHeaders::setpos;
	using KalaHeaders::addrot;
	using KalaHeaders::setrot;
	using KalaHeaders::getroteuler;
	using KalaHeaders::getdirfront;
	using KalaHeaders::getdirright;
	using KalaHeaders::getdirup;
	using KalaHeaders::view;
	using KalaHeaders::wrap;
	using KalaHeaders::isnear;
	
	using KalaGraphics::Utils::KalaGraphicsRegistry;

	class LIB_API Camera
	{
	public:
		static inline KalaGraphicsRegistry<Camera> registry{};

		static Camera* Initialize(
			const string& cameraName,
			vec2 framebufferSize,
			f32 fov,
			f32 speed,
			const vec3& pos = {},
			const vec3& rot = {});

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty
			if (newName.empty()) return;
			//skip if name is too long
			if (newName.length() > 50) return;
			//skip if name is already same
			if (newName == name) return;

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
			inc.y = delta.x * sensitivity;
			//hard-locked roll
			inc.z = 0.0f;
			
			//flattens axes to 0 if any of them are near 0
			if (isnear(inc.x)) inc.x = 0.0f;
			if (isnear(inc.y)) inc.y = 0.0f;
			if (isnear(inc.z)) inc.z = 0.0f;
			
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

		//Called inside resize callback to ensure camera aspect ratio always stays valid
		inline void SetAspectRatio(f32 size)
		{
			aspectRatio = clamp(size, 0.001f, 10.0f);
		}
		inline f32 GetAspectRatio() const { return aspectRatio; }

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
			vec3 up = GetUp();
			
			return view(pos, pos + front, up);
		};

		~Camera();
	private:
		bool isInitialized{};

		string name{};

		u32 ID{};

		f32 fov{};
		f32 speed{};

		f32 aspectRatio{};

		f32 nearClip = 0.01f;
		f32 farClip = 512.0;
		f32 sensitivity = 0.1f;

		Transform3D transform{};
	};
}