//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Engine
	{
	public:
		static inline string name = "Elypso engine";
		static inline string version = "0.0.11 Prototype";
		static inline string docsPath;
		static inline string filesPath;
		static inline string enginePath;

		static inline bool startedWindowLoop;

		static void PreInitializePathChecks();

		static void InitializeEngine();
		static void RunEngine();

		static void CreateErrorPopup(const char* errorTitle = "INSERT ERROR TITLE", const char* errorMessage = "INSERT ERROR MESSAGE");

		static void Shutdown();
	};
}