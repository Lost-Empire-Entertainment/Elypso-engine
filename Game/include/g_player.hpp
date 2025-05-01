//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

//engine
#include "gameobject.hpp"

namespace GameTemplate
{
	using std::shared_ptr;

	using Graphics::Shape::GameObject;

	class G_Player
	{
	public:
		static inline float walkSpeed = 1.0f;
		static inline float speedAmplify = 10.0f;
		static inline float jumpStrength = 3.0f;
		static inline float maxVelocity = 1.0f;

		static inline shared_ptr<GameObject> model_player = nullptr;
		static inline shared_ptr<GameObject> camera = nullptr;

		/// <summary>
		/// Set up player and camera.
		/// </summary>
		static void Initialize();
		/// <summary>
		/// Correctly update camera position and model rotation.
		/// </summary>
		static void UpdatePlayerPos();

		static void UpdateLastRotation();
	private:
		/// <summary>
		/// All player movement inputs.
		/// </summary>
		static void MovePlayer();
	};
}