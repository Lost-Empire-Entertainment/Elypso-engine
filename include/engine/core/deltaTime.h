#pragma once

#ifndef CORE_DELTATIME_H
#define CORE_DELTATIME_H

namespace Core
{
	class DeltaTime
	{
	public:
		static inline float deltaTime;
		static void UpdateDeltaTime();
	private:
		static inline float lastFrame;
	};
}

#endif