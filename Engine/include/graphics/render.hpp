//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

//engine
#include "gameobject.hpp"
#include "input.hpp"
#include "shader.hpp"

using std::vector;
using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;
using Core::Input;

namespace Graphics
{
	class Render
	{
	public:
		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static inline vec3 directionalDirection = vec3(-0.2f, -1.0f, -0.3f);
		static inline vec3 directionalDiffuse = vec3(0.4f, 0.4f, 0.4f);
		static inline float directionalIntensity = 1.0f;

		static inline bool useMonitorRefreshRate;

		static inline GLFWwindow* window;

		static inline int windowedWidth;
		static inline int windowedHeight;
		static inline mat4 projection;
		static inline mat4 view;

		static Input camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void SetWindowNameAsUnsaved(bool state);
		static void WindowLoop();
	private:
		static inline unsigned int windowWidth = 1280;
		static inline unsigned int windowHeight = 720;

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}