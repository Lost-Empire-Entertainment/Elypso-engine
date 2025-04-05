//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAUTILS_DLL_EXPORT
		#define KALAUTILS_API __declspec(dllexport)
	#else
		#define KALAUTILS_API __declspec(dllimport)
	#endif
#else
	#define KALAUTILS_API
#endif

#include <initializer_list>
#include <unordered_map>
#include <string>

#include "osutils.hpp"
#include "enums.hpp"

namespace KalaKit
{
	using std::initializer_list;
	using std::unordered_map;
	using std::string;

	class KALAUTILS_API InputUtils
	{
	public:
		/// <summary>
		/// Return true if assigned key is currently held down.
		/// </summary>
		static bool IsKeyHeld(Key key);
		/// <summary>
		/// Return true if assigned key was pressed.
		/// </summary>
		static bool IsKeyPressed(Key key);

		/// <summary>
		/// Return true after assigned initializer list of keys is held 
		/// up to last key in correct order, and if last key is pressed.
		/// Must have atleast two keys assigned.
		/// </summary>
		static bool IsComboPressed(initializer_list<Key> keys);

		/// <summary>
		/// Return true if left or right mouse button was double-clicked.
		/// </summary>
		static bool IsMouseKeyDoubleClicked();

		/// <summary>
		/// Return true if user is holding left or right mouse button and dragging mouse.
		/// </summary>
		static bool IsMouseDragging();

		/// <summary>
		/// Get current mouse position relative to the client area (top-left = 0,0).
		/// Coordinates are in pixels.
		/// </summary>
		static POS GetMousePosition();
		/// <summary>
		/// Assign a new mouse position.
		/// </summary>
		static void SetMousePosition(POS newMousePosition);

		/// <summary>
		/// Get how much the cursor moved on screen (in client space) since the last frame.
		/// This uses absolute screen-based movement, affected by OS acceleration and DPI.
		/// </summary>
		static POS GetMouseDelta();
		/// <summary>
		/// Assign a new mouse delta.
		/// </summary>
		static void SetMouseDelta(POS newMouseDelta);

		/// <summary>
		/// Get raw, unfiltered mouse movement from the hardware since the last frame.
		/// Not affected by DPI, sensitivity, or OS mouse settings. Ideal for FPS camera control.
		/// </summary>
		static POS GetRawMouseDelta();
		/// <summary>
		/// Assign a new mouse raw mouse delta.
		/// </summary>
		static void SetRawMouseDelta(POS newMouseRawDelta);

		/// <summary>
		/// Get how many scroll steps the mouse wheel moved since the last frame.
		/// Positive = scroll up, Negative = scroll down
		/// </summary>
		static int GetMouseWheelDelta();
		/// <summary>
		/// Assign a new mouse wheel delta.
		/// </summary>
		/// <param name="newMouseWheelDelta"></param>
		static void SetMouseWheelDelta(int newMouseWheelDelta);

		/// <summary>
		/// Return true if cursor is not hidden.
		/// </summary>
		static bool IsMouseVisible();
		/// <summary>
		/// Allows to set the visibility state of the cursor, if true then the cursor is visible.
		/// </summary>
		static void SetMouseVisibility(bool newMouseVisibleState);

		/// <summary>
		/// Return true if the cursor is locked to the center of the window.
		/// </summary>
		static bool IsMouseLocked();
		/// <summary>
		/// Allows to set the lock state of the cursor, if true 
		/// then the cursor is locked to the center of the window.
		/// </summary>
		static void SetMouseLockState(bool newLockedState);

		/// <summary>
		/// Resets all frame-specific variables to defaults.
		/// </summary>
		static void ResetFrameInput();

		/// <summary>
		/// Locks cursor to the center of the window.
		/// Should not be called manually.
		/// </summary>
		static void LockCursorToCenter();

		/// <summary>
		/// Used for setting a key state for any of the keys.
		/// Should not be called manually.
		/// </summary>
		/// <param name="key"></param>
		/// <param name="isDown"></param>
		static void SetKeyState(Key key, bool isDown)
		{
			keyHeld[key] = isDown;
			if (isDown) keyPressed[key] = true;
		}
	private:
		/// <summary>
		/// Whether to show the mouse or not.
		/// </summary>
		static inline bool isMouseVisible = true;
		/// <summary>
		/// Whether to lock the mouse to the center of the screen or not.
		/// </summary>
		static inline bool isMouseLocked = false;

		//Where the cursor is on screen or in window.
		static inline POINT mousePosition = { 0, 0 };
		//How much the cursor moved since the last frame.
		static inline POINT mouseDelta = { 0, 0 };
		//Raw, unfiltered mouse move since last frame.
		static inline POINT rawMouseDelta = { 0, 0 };

		//How many steps scrollwheel scrolled since last frame.
		static inline int mouseWheelDelta = 0;

		static inline unordered_map<Key, bool> keyHeld;
		static inline unordered_map<Key, bool> keyPressed;

		static string ToString(Key key);
	};
}