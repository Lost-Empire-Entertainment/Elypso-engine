//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAPHYSICS_DLL_EXPORT
		#define KALAPHYSICS_API __declspec(dllexport)
	#else
		#define KALAPHYSICS_API __declspec(dllimport)
	#endif
#else
	#define KALAPHYSICS_API
#endif

namespace KalaKit::Physics::Simulation
{
	class KALAPHYSICS_API Solver
	{
	public:
		virtual void Solve(float deltaTime, int iterations) = 0;
		virtual void Clear() = 0;
		virtual ~Solver() = default;
	};
}