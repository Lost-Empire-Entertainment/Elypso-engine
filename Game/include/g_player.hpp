//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace GameTemplate
{
	class G_Player
	{
	public:
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