//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Game
{
	using std::string;

	class Game_Core
	{
	public:
		/// <summary>
		/// Initialize the game.
		/// </summary>
		static void Initialize();

		/// <summary>
		/// Handles game render loop, physics and delta time updates.
		/// </summary>
		static void Run();

		/// <summary>
		/// As long as game is not closed from top X button
		/// or with Alt + F4 this will remain true.
		/// </summary>
		static bool ShouldClose();

		/// <summary>
		/// Shut down game.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Toggle if right mouse key needs to be held to rotate camera.
		/// </summary>
		/// <param name="rightMouseState"></param>
		static void SetRightClickState(bool rightClickState);

		/// <summary>
		/// Toggle all camera movement on and off.
		/// </summary>
		static void SetMovementState(bool movementState);

		/// <summary>
		/// Toggle Y camera movement on and off.
		/// </summary>
		static void SetYMovementState(bool yMovementState);
	};
}
