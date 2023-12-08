#pragma once

#ifndef CORE_INPUT_H
#define CORE_INPUT_H

//external
#include <glad.h>

//engine
#include "render.h"

#include <string>
#include <unordered_map>

using namespace std;
using namespace Graphics;

namespace Core
{
	class InputManager
	{
	public:
		enum class Key : char
		{
			Escape,
			Enter,
			Forwards,
			Backwards,
			Left,
			Right
		};

		static inline bool firstMouse = true;
		static inline float lastX = Render::SCR_WIDTH / 2.0f;
		static inline float lastY = Render::SCR_HEIGHT / 2.0f;

		static inline unordered_map<Key, int> key;

		static void InputSetup();
		static void ProcessInput(GLFWwindow* window);
		static void Mouse_Callback(GLFWwindow* window, double xPos, double yPos);
		static void Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);
	};
}

#endif