//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace GameTemplate
{
	class G_States
	{
	public:
		//ui states

		//Are we currently in the main menu?
		static inline bool isInMainMenu = false;
		//Are we currently in the pause menu?
		static inline bool isInPauseMenu = false;
		//Should the debug window be rendered?
		static inline bool renderDebugWindow = false;
		//Is the game console currently open?
		static inline bool isConsoleOpen = false;

		//player states

		//Is the player allowed to move with keyboard?
		static inline bool canMove = true;
		//Is the camera allowed to move with keyboard?
		static inline bool movementState = false;
		//Is the camera allowed to rotate with mouse?
		static inline bool rotationState = true;
		//Is the camera allowed to go up and down?
		static inline bool yMovementState = false;
		//Is the camera only rotatable while holding right mouse key?
		static inline bool rightClickState = false;
	};
}