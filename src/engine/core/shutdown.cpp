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
#include "glfw3.h"

//engine
#include "console.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "shutdown.hpp"

using Graphics::Render;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void ShutdownManager::Shutdown()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::SHUTDOWN,
			Type::INFO,
			"Cleaning up resources...\n");

		Render::Shutdown();

		//de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &Render::VAO);
		glDeleteBuffers(1, &Render::VBO);
		glDeleteBuffers(1, &Render::EBO);

		//clean all glfw resources after program is closed
		glfwTerminate();

		ConsoleManager::WriteConsoleMessage(
			Caller::SHUTDOWN,
			Type::SUCCESS,
			"Shutdown complete!\n");
	}
}