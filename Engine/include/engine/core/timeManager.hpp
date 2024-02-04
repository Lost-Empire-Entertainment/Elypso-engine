//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration;

namespace Core
{
	class TimeManager 
	{
	public:
		static inline double displayedFPS;
		static inline double deltaTime;

		static void InitializeDeltaTime();
		static void UpdateDeltaTime();
	private:
		static inline int frame_count;
		static inline high_resolution_clock::time_point start_time;
		static inline high_resolution_clock::time_point last_frame_time;
		static inline duration<double> frame_duration;

		//smooth displayed fps update
		static inline int smoothed_frame_count;
		static inline double smoothing_interval;
		static inline high_resolution_clock::time_point last_smoothed_update;
		static inline high_resolution_clock::time_point current_time;
		static inline duration<double> elapsed_seconds;
	};
}