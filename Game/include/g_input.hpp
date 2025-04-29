//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

namespace GameTemplate
{
	using std::string;
	using std::unordered_map;

	struct KeyState
	{
		int key = -1;
		bool wasDown = false;
		bool isDown = false;
	};

	class G_Input
	{
	public:
		static void Initialize();
		static void UpdateKeyStates();
		static void UpdateMouse();

		/// <summary>
		/// Returns true if key is pressed once, not held.
		/// </summary>
		static bool IsPressed(const string& name);
		/// <summary>
		/// Returns true if key is currently held.
		/// </summary>
		static bool IsHeld(const string& name);
	private:
		static inline unordered_map<string, KeyState> keyStates;
	};
}