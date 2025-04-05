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

#include <string>

#include "osutils.hpp"
#include "enums.hpp"

namespace KalaKit
{
	using std::string;

	class KALAUTILS_API WindowUtils
	{
	public:
		static inline WINDOW window;

		/// <summary>
		/// Get the currently assigned debug type.
		/// </summary>
		static DebugType GetDebugType();
		/// <summary>
		/// Used for printing all input actions or specific ones 
		/// to console with cout if a console is attached to the window.
		/// You MUST be in Debug mode or else these messages will not be printed.
		/// </summary>
		static void SetDebugType(DebugType newDebugType);

		static bool GetWindowFocusRequiredState();
		/// <summary>
		/// Used for setting window focus required state.
		/// If true, then the assigned window needs to be focused
		/// for any input to be able to be passed to it.
		/// </summary>
		static void SetWindowFocusRequiredState(bool newWindowFocusRequiredState);

		/// <summary>
		/// Assign a title to the window.
		/// </summary>
		static void SetWindowTitle(const string& title);

		/// <summary>
		/// Set the window to one of the possible states.
		/// </summary>
		static void SetWindowState(WindowState state);

		/// <summary>
		/// Return true if the window is borderless.
		/// </summary>
		static bool IsWindowBorderless();
		/// <summary>
		/// Allows to set the borderless state of the window,
		/// if true, then the window will be set to borderless.
		/// </summary>
		static void SetWindowBorderlessState(bool newBorderlessState);

		/// <summary>
		/// Return true if the window is hidden.
		/// </summary>
		static bool IsWindowHidden();
		/// <summary>
		/// Allows to set the hidden state of the window,
		/// if true, then the window will be set to hidden.
		/// </summary>
		static void SetWindowHiddenState(bool newWindowHiddenState);

		/// <summary>
		/// Gets the position of the window.
		/// </summary>
		static POS GetWindowPosition();
		/// <summary>
		/// Sets the position of the window.
		/// </summary>
		static void SetWindowPosition(int width, int height);

		/// <summary>
		/// Gets the total outer size (includes borders and top bar)
		/// </summary>
		static POS GetWindowFullSize();
		/// <summary>
		/// Sets the total outer size of the window (includes borders and top bar)
		/// </summary>
		static void SetWindowFullSize(int width, int height);

		/// <summary>
		/// Gets the drawable/client area (without borders and top bar)
		/// </summary>
		static POS GetWindowContentSize();
		/// <summary>
		/// Sets the drawable/client area (without borders and top bar)
		/// </summary>
		static void SetWindowContentSize(int width, int height);

		/// <summary>
		/// Get window maximum allowed size.
		/// </summary>
		static POS GetWindowMaxSize();
		/// <summary>
		/// Get window minimum allowed size.
		/// </summary>
		static POS GetWindowMinSize();
		/// <summary>
		/// Set window minimum and maximum allowed size.
		/// </summary>
		static void SetMinMaxSize(
			int maxWidth,
			int maxHeight,
			int minWidth,
			int minHeight);
	private:
		/// <summary>
		/// Used for checking whether window should be focused or not 
		/// to enable any input to be registered.
		/// </summary>
		static inline bool isWindowFocusRequired;

		/// <summary>
		/// Used for checking if this window is visible to the user or not.
		/// If true, then this window is visible.
		/// </summary>
		static inline bool isWindowVisible = true;

		/// <summary>
		/// Used for checking if this window is borderless or not.
		/// If true, then this window is borderless.
		/// </summary>
		static inline bool isWindowBorderless = false;

		/// <summary>
		/// Currently assigned debug type
		/// </summary>
		static inline DebugType debugType = DebugType::DEBUG_NONE;

		/// <summary>
		/// Store original window flags when switching 
		/// between borderless and non-borderless
		/// </summary>
		static inline LONG originalStyle = 0;
		/// <summary>
		/// Store original size, position and show state 
		/// when switching between borderless and non-borderless
		/// </summary>
		static inline WINDOWPLACEMENT originalPlacement = { sizeof(WINDOWPLACEMENT) };

		static inline int maxWidth = 7680;
		static inline int maxHeight = 4320;
		static inline int minWidth = 800;
		static inline int minHeight = 600;
	};
}