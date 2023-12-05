#pragma once

//external
#include <glfw3.h>

#include <string>
#include <unordered_map>

namespace Core
{
	class InputManager
	{
	public:
		enum class Key : char
		{
			Escape,
			Enter
		};

		static inline std::unordered_map<Key, int> key;

		static void InputSetup();
		static void ProcessInput(GLFWwindow* window);
	};
}