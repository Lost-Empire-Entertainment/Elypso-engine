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

#pragma once

#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace Core
{
	class Engine
	{
	public:
		static inline string name;
		static inline string version;
		static inline double fps;
		static inline double displayedFPS;
		static inline double targetUpdateInterval;

		static void InitializeEngine();
		static void RunEngine();
	private:
		static inline high_resolution_clock::time_point startTime;
		static inline high_resolution_clock::time_point endTime;
		static inline high_resolution_clock::time_point startFrameTime;
		static inline high_resolution_clock::time_point endFrameTime;
		static inline high_resolution_clock::time_point lastUpdate;

		static void CalculateFPS();
		static void CalculateDisplayedFPS();
	};
}