//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/math_utils.hpp"

#include "core/en_registry.hpp"

namespace ElypsoEngine::GameObject
{
	using std::string;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::Transform3D;

	using ElypsoEngine::Core::EngineRegistry;

	class Camera
	{
	public:
		static EngineRegistry<Camera>& GetRegistry();

		static Camera* Initialize(
			const string& cameraName,
			const vec3& pos,
			const vec3& rot,
			vec2 framebufferSize,
			f32 fov,
			f32 speed);

		bool IsInitialized() const;

		u32 GetID() const;

		void SetName(const string& newName);
		const string& GetName();

		//Handle camera rotation based off of mouse movement
		void UpdateCameraRotation(vec2 delta);

		void SetFOV(f32 newFOV);
		f32 GetFOV() const;

		void SetNearClip(f32 newNearClip);
		f32 GetNearClip() const;

		void SetFarClip(f32 newFarClip);
		f32 GetFarClip() const;

		void SetSpeed(f32 newSpeed);
		f32 GetSpeed() const;

		void SetSensitivity(f32 newSens);
		f32 GetSensitivity() const;
		
		vec3 GetFront();
		vec3 GetRight();
		vec3 GetUp();

		//Increments position over time
		void AddPos(const vec3& deltaPos);
		//Snaps to given position
		void SetPos(const vec3& newPos);
		vec3 GetPos();

		//Increments rotation over time
		void AddRot(const vec3& deltaRot);
		//Snaps to given rotation
		void SetRot(const vec3& newRot);
		vec3 GetRot();
		
		mat4 GetViewMatrix();
		//Returns aspect-ratio-correct ortographic matrix based off of inserted viewport
		mat4 GetOrthographicMatrix(vec2 viewportSize);
		//Returns aspect-ratio-correct perspective matrix based off of inserted viewport
		mat4 GetPerspectiveMatrix(vec2 viewportSize);

		~Camera();
	private:
		bool isInitialized{};

		string name{};

		u32 ID{};

		f32 fov{};
		f32 speed{};

		f32 nearClip = 0.001f;
		f32 farClip = 512.0f;
		f32 sensitivity = 0.1f;

		Transform3D transform{};
	};
}