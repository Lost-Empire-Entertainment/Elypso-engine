//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glfw3.h"

//engine
#include "render.hpp"
#include "game_core.hpp"

//game
#include "g_input.hpp"
#include "g_states.hpp"
#include "g_player.hpp"

using Graphics::Render;
using Game::Game_Core;

namespace GameTemplate
{
	static GLFWwindow* window = nullptr;

	void G_Input::Initialize()
	{
		window = Render::window;

		keyStates["Front"] = { GLFW_KEY_W };
		keyStates["Left"] = { GLFW_KEY_A };
		keyStates["Back"] = { GLFW_KEY_S };
		keyStates["Right"] = { GLFW_KEY_D };
		keyStates["Jump"] = { GLFW_KEY_SPACE };
		keyStates["Crouch"] = { GLFW_KEY_LEFT_CONTROL };
		keyStates["Sprint"] = { GLFW_KEY_LEFT_SHIFT };
		keyStates["Interact"] = { GLFW_KEY_E };
		keyStates["Pause"] = { GLFW_KEY_ESCAPE };
		keyStates["Console"] = { GLFW_KEY_PAGE_UP };
	}

	void G_Input::UpdateKeyStates()
	{
		for (auto& [name, state] : keyStates)
		{
			state.wasDown = state.isDown;
			state.isDown = glfwGetKey(window, state.key) == GLFW_PRESS;
		}

		if (IsPressed("Pause"))
		{
			G_States::isInPauseMenu = !G_States::isInPauseMenu;
			if (G_States::isInPauseMenu)
			{
				G_States::canMove = false;
				G_States::rotationState = false;
			}
			else
			{
				G_States::canMove = true;
				G_States::rotationState = true;
			}
			Game_Core::SetRotationState(G_States::rotationState);
		}

		if (IsPressed("Console"))
		{
			G_States::isConsoleOpen = !G_States::isConsoleOpen;
		}

		G_Player::UpdateLastRotation();
	}
	void G_Input::UpdateMouse()
	{
		if (!G_States::rotationState)
		{
			glfwSetInputMode(Render::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(Render::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	bool G_Input::IsPressed(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown
			&& !it->second.wasDown;
	}

	bool G_Input::IsHeld(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown;
	}
}