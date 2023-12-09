//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glad.h"

//engine
#include "input.h"
#include "console.h"
#include "render.h"

using namespace Graphics;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void InputManager::InputSetup()
	{
		key[Key::Escape] = GLFW_KEY_ESCAPE;
		key[Key::Enter] = GLFW_KEY_ENTER;
	}

	void InputManager::ProcessInput(GLFWwindow* window)
	{
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Escape])) == GLFW_PRESS)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::SUCCESS,
				"User pressed ESC key to shut down engine...\n\n");

			glfwSetWindowShouldClose(window, true);
		}
	}
}