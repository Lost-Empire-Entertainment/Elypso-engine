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

#include <vector>

//external
#include "glm.hpp"

namespace KalaKit::Physics::Collision
{
	using std::vector;
	using glm::vec3;

	class KALAPHYSICS_API FaceClipping
	{
	public:
		vector<vec3> ClipFaceAgainstPlane(
			const vector<vec3>& face,
			const vec3& planeNormal,
			float planeOffset);
	};
}