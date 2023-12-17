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

//engine
#include "core.hpp"
#include "console.hpp"
#include "shutdown.hpp"

using Core::Engine;
using Core::ShutdownManager;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

int main()
{
	ConsoleManager::WriteConsoleMessage(
		Caller::ENGINE,
		Type::INFO,
		"Elypso engine " + Engine::version +"\n" +
		"Copyright (C) Greenlaser 2023\n\n",
		true);

	Engine::InitializeEngine();

	Engine::RunEngine();

	ShutdownManager::Shutdown();
	return 0;
}