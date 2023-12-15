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

#include <chrono>

using std::chrono::high_resolution_clock;

namespace Core
{
	class TimeManager 
	{
	public:
		static inline double fps;
		static inline double displayedFPS;
		static inline float deltaTime;
		static inline float targetDT;
		static inline high_resolution_clock::time_point lastTime;

		static void UpdateDeltaTime();
	private:
		static inline float lastFrame;
		static inline high_resolution_clock::time_point lastUpdate;
	};
}