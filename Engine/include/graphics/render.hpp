//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <string>

//engine
#include "camera.hpp"

using glm::vec3;
using glm::mat4;
using std::map;
using std::string;

using Graphics::Camera;

namespace Graphics
{
	class Render
	{
	public:
		static inline map<int, string> aspectRatio
		{
			{0, "Unlocked"},
			{1, "16:9"},
			{2, "16:10"},
			{3, "21:9"},
			{4, "32:9"},
			{5, "4:3"},
		};
		static inline int currentIndex = 0;

		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static inline vec3 directionalDirection = vec3(-0.2f, -1.0f, -0.3f);
		static inline vec3 directionalDiffuse = vec3(0.4f, 0.4f, 0.4f);
		static inline float directionalIntensity = 1.0f;

		static inline GLFWwindow* window;

		static inline mat4 projection;
		static inline mat4 view;

		static Camera camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void SetWindowNameAsUnsaved(bool state);
		static void WindowLoop();
		static void RenderToImguiWindow();
	private:
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void FramebufferSetup();
		static void ContentSetup();
	};
}