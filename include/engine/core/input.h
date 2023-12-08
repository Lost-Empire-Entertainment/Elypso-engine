#pragma once

#ifndef CORE_INPUT_H
#define CORE_INPUT_H

//external
#include <glfw3.h>

#include <string>
#include <unordered_map>

using namespace std;

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

		static inline unordered_map<Key, int> key;

		static void InputSetup();
		static void ProcessInput(GLFWwindow* window);
	};
}

#endif