//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <string>

//engine
#include "camera.hpp"

namespace Graphics
{
	using glm::vec3;
	using glm::mat4;
	using std::map;
	using std::string;

	using Graphics::Camera;

	class Render
	{
	public:
		static inline GLFWwindow* window;

		static inline mat4 projection;
		static inline mat4 view;

		static Camera camera;

		static inline unsigned int textureColorbuffer;
		static inline unsigned int rbo;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void SetWindowNameAsUnsaved(bool state);
		static void WindowLoop();
	private:
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
		static void SkyboxSetup();

#if ENGINE_MODE
		static void FramebufferSetup();
#endif
	};
}