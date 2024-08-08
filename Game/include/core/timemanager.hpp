//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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